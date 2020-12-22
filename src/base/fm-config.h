/*
 *      fm-config.h
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2009 Juergen Hoetzel <juergen@archlinux.org>
 *      Copyright 2012-2014 Andriy Grytsenko (LStranger) <andrej@rep.kiev.ua>
 *      Copyright 2020-2021 Damian Ivanov <damianatorrpm@gmail.com>
 * 
 *      This file is a part of the Libfm library.
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2.1 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 * USA
 */

#pragma once

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define FM_CONFIG_TYPE              (fm_config_get_type())
#define FM_CONFIG(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            FM_CONFIG_TYPE, FmConfig))
#define FM_CONFIG_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass),\
            FM_CONFIG_TYPE, FmConfigClass))
#define FM_IS_CONFIG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            FM_CONFIG_TYPE))
#define FM_IS_CONFIG_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass),\
            FM_CONFIG_TYPE))

typedef struct _FmConfig            FmConfig;
typedef struct _FmConfigClass       FmConfigClass;

/* this enum is used by FmDndDest but we save it nicely in config so have it here */

/**
 * FmDndDestDropAction:
 * @FM_DND_DEST_DROP_AUTO: move if source and destination are on the same file system, copy otherwise
 * @FM_DND_DEST_DROP_COPY: copy
 * @FM_DND_DEST_DROP_MOVE: move
 * @FM_DND_DEST_DROP_ASK: open popup to let user select desired action
 *
 * selected behavior when files are dropped on destination widget.
 */
typedef enum
{
    FM_DND_DEST_DROP_AUTO,
    FM_DND_DEST_DROP_COPY,
    FM_DND_DEST_DROP_MOVE,
    FM_DND_DEST_DROP_ASK
} FmDndDestDropAction;

struct _FmConfig
{
    /*< private >*/
    GObject parent;
    GSettings *settings;

    /*< public >*/
    char* terminal;
    char* archiver;

    gint big_icon_size;
    gint small_icon_size;
    gint pane_icon_size;
    gint thumbnail_size;
    gint thumbnail_max;
    gint auto_selection_delay;
    gint drop_default_action;

    gboolean single_click;
    gboolean use_trash;
    gboolean confirm_del;
    gboolean confirm_trash;
    gboolean show_thumbnail;
    gboolean thumbnail_local;
    gboolean si_unit;
    gboolean advanced_mode;
    gboolean force_startup_notify;
    gboolean backup_as_hidden;
    gboolean no_usb_trash;
    gboolean no_child_non_expandable;
    gboolean show_full_names;
    gboolean shadow_hidden;

    gboolean places_home;
    gboolean places_desktop;
    gboolean places_applications;
    gboolean places_trash;
    gboolean places_root;
    gboolean places_computer;
    gboolean places_network;
    gboolean places_unmounted;

    gboolean only_user_templates;
    gboolean template_run_app;
    gboolean template_type_once;
    gboolean defer_content_test;
    gboolean quick_exec;

    gchar **modules_blacklist;
    gchar **modules_whitelist;
    /*< private >*/
    gchar **system_modules_blacklist; /* concatenated from system, don't save! */
    /*< public >*/

    gchar *list_view_size_units;
    gchar *format_cmd;

    gboolean smart_desktop_autodrop;
    gchar *saved_search;
    /*< private >*/
    gpointer _reserved1; /* reserved space for updates until next ABI */
    gpointer _reserved2;
    gpointer _reserved3;
    gpointer _reserved4;
    gpointer _reserved5;
    gpointer _reserved6;
    gpointer _reserved7;
};

/**
 * FmConfigClass
 * @parent_class: the parent class
 * @changed: the class closure for the #FmConfig::changed signal
 */
struct _FmConfigClass
{
    GObjectClass parent_class;
    void (*changed)(FmConfig* cfg);
};

/* global config object */
extern FmConfig* fm_config;

GType       fm_config_get_type      (void);
FmConfig*   fm_config_new           (void);

void fm_config_load(FmConfig *cfg);

void fm_config_save(FmConfig* cfg);

void fm_config_reset(FmConfig *cfg, const char *changed_key);

void fm_config_emit_changed(FmConfig* cfg, const char* changed_key);

G_END_DECLS