/*
 *      fm-job.h
 *      
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#ifndef __FM_JOB_H__
#define __FM_JOB_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <stdarg.h>

G_BEGIN_DECLS

#define FM_TYPE_JOB				(fm_job_get_type())
#define FM_JOB(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			FM_TYPE_JOB, FmJob))
#define FM_JOB_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			FM_TYPE_JOB, FmJobClass))
#define FM_IS_JOB(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			FM_TYPE_JOB))
#define FM_IS_JOB_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			FM_TYPE_JOB))

typedef struct _FmJob			FmJob;
typedef struct _FmJobClass		FmJobClass;

typedef gpointer (*FmJobCallMainThreadFunc)(FmJob* job, gpointer user_data);

enum _FmJobErrorSeverity
{
    FM_JOB_ERROR_WARNING, /* not an error, just a warning */
    FM_JOB_ERROR_MILD, /* no big deal, can be ignored most of the time */
    FM_JOB_ERROR_MODERATE, /* moderate errors */
    FM_JOB_ERROR_SEVERE, /* severe errors, whether to abort operation depends on error handlers */
    FM_JOB_ERROR_CRITICAL /* critical errors, the operation is aborted */
};
typedef enum _FmJobErrorSeverity FmJobErrorSeverity;

enum _FmJobErrorAction
{
    FM_JOB_CONTINUE, /* ignore the error and continue remaining work */
    FM_JOB_RETRY, /* retry the previously failed operation. (not every kind of job support this) */
    FM_JOB_ABORT /* abort the whole job */
};
typedef enum _FmJobErrorAction FmJobErrorAction;

struct _FmJob
{
	GObject parent;
	gboolean cancel : 1;
	gboolean running : 1;

	/* optional, should be created if the job uses gio */
	GCancellable* cancellable;

	/* optional, used when blocking the job to call a callback in 
	 * main thread is needed. */
	GMutex* mutex;
	GCond* cond;
};

struct _FmJobClass
{
	GObjectClass parent_class;

	void (*finished)(FmJob* job);
	FmJobErrorAction (*error)(FmJob* job, GError* err, FmJobErrorSeverity severity);
	void (*cancelled)(FmJob* job);
	gint (*ask)(FmJob* job, const char* question, gint options);

	gboolean (*run_async)(FmJob* job);
	gboolean (*run)(FmJob* job);
	void (*cancel)(FmJob* job);
};


/* Base type of all file I/O jobs.
 * not directly called by applications. */

/* FmJob*	fm_job_new			(void); */
GType	fm_job_get_type		(void);

/* return TRUE if the job is already cancelled */
gboolean fm_job_is_cancelled(FmJob* job);

/* return TRUE if the job is still running */
gboolean fm_job_is_running(FmJob* job);


/* FIXME: requiring g_object_unref() is actually a better idea
 * due to API consistency. Need to re-think this part. */

/* Run a job asynchronously in another working thread, and 
 * emit 'finished' signal in the main thread after its termination.
 * There is no need to call g_object_unref on a job running in 
 * async fashion. The job object will be unrefed in idle handler
 * automatically shortly after its termination.
 * The default implementation of FmJob::run_async() create a working
 * thread in thread pool, and calls FmJob::run() in it.
 */
gboolean fm_job_run_async(FmJob* job);

/* Run a job in current thread in a blocking fashion.
 * A job running synchronously with this function should be unrefed
 * later with g_object_unref when no longer needed. */
gboolean fm_job_run_sync(FmJob* job);

/* Run a job in current thread in a blocking fashion and an additional 
 * mainloop being created to prevent blocking of user interface.
 * A job running synchronously with this function should be unrefed
 * later with g_object_unref when no longer needed. */
gboolean fm_job_run_sync_with_mainloop(FmJob* job);

/* Cancel the running job. can be called from any thread. */
void fm_job_cancel(FmJob* job);

/* Following APIs are private to FmJob and should only be used in the
 * implementation of classes derived from FmJob.
 * Besides, they should be called from working thread only */
gpointer fm_job_call_main_thread(FmJob* job, 
					FmJobCallMainThreadFunc func, gpointer user_data);

/* Used by derived classes to implement FmJob::run() using gio inside.
 * This API tried to initialize a GCancellable object for use with gio and
 * should only be called once in the constructor of derived classes which
 * require the use of GCancellable. */
void fm_job_init_cancellable(FmJob* job);

/* Used to implement FmJob::run() using gio inside.
 * This API tried to initialize a GCancellable object for use with gio.
 * Prior to calling this API, fm_job_init_cancellable() should be
 * called first to initiate GCancellable object. Otherwise NULL is returned. */
GCancellable* fm_job_get_cancellable(FmJob* job);

/* Let the job use an existing cancellable object.
 * This can be used when you wish to share a cancellable object
 * among different jobs.
 * This should only be called before the job is launched. */
void fm_job_set_cancellable(FmJob* job, GCancellable* cancellable);

/* only call this at the end of working thread if you're going to 
 * override FmJob::run() and use your own multi-threading mechnism. */
void fm_job_finish(FmJob* job);

void fm_job_emit_finished(FmJob* job);

void fm_job_emit_cancelled(FmJob* job);

/* Emit an 'error' signal to notify the main thread when an error occurs.
 * The return value of this function is the return value returned by
 * the connected signal handlers.
 * If severity is FM_JOB_ERROR_CRITICAL, the returned value is ignored and
 * fm_job_cancel() is called to abort the job. Otherwise, the signal
 * handler of this error can return FM_JOB_RETRY to ask for retrying the
 * failed operation, return FM_JOB_CONTINUE to ignore the error and
 * continue the remaining job, or return FM_JOB_ABORT to abort the job.
 * If FM_JOB_ABORT is returned by the signal handler, fm_job_cancel
 * will be called in fm_job_emit_error().
 */
FmJobErrorAction fm_job_emit_error(FmJob* job, GError* err, FmJobErrorSeverity severity);

gint fm_job_ask(FmJob* job, const char* question, ...);
gint fm_job_askv(FmJob* job, const char* question, gchar* const *options);
gint fm_job_ask_valist(FmJob* job, const char* question, va_list options);

G_END_DECLS

#endif /* __FM-JOB_H__ */
