/*
 *      fm-config.c
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
 *      Copyright 2009 Juergen Hoetzel <juergen@archlinux.org>
 *      Copyright 2012-2014 Andriy Grytsenko (LStranger) <andrej@rep.kiev.ua>
 *      Copyright 2016 Mamoru TASAKA <mtasaka@fedoraproject.org>
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
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * SECTION:fm-config
 * @short_description: Configuration support for applications that use libfm.
 * @title: FmConfig
 *
 * @include: libfm/fm.h
 *
 * The #FmConfig represents basic configuration options that are used by
 * libfm classes and methods.
 */

#include "fm-config.h"
#include "fm-utils.h"
#include <stdio.h>

enum
{
    CHANGED,
    N_SIGNALS
};

/* global config object */
FmConfig* fm_config = NULL;

static guint signals[N_SIGNALS];

static void fm_config_finalize              (GObject *object);

G_DEFINE_TYPE(FmConfig, fm_config, G_TYPE_OBJECT);

static int drop_action_from_str(FmConfig* cfg)
{
    gint res;
    char *str = g_settings_get_string(cfg->settings, "drop-action");
    if (g_strcmp0(str, "Auto") == 0)
        res = FM_DND_DEST_DROP_AUTO;
    else if (g_strcmp0(str, "Copy") == 0)
        res = FM_DND_DEST_DROP_COPY;
    else if (g_strcmp0(str, "Move") == 0)
        res = FM_DND_DEST_DROP_MOVE;
    else if (g_strcmp0(str, "Ask") == 0)
        res = FM_DND_DEST_DROP_ASK;     
    else
        g_error("Setting outside of gsettings choice boundary.");

    g_free(str);
    return res;  
}
static void on_external_settings_change(GSettings *settings, gchar *key,
                                        gpointer data)
{
    FmConfig *self = FM_CONFIG(data);
    if (g_strcmp0(key, "drop-action") == 0)
        self->drop_default_action = drop_action_from_str(self);     

    else if (g_strcmp0(key, "archiver") == 0)
        self->archiver = g_settings_get_string(settings, key);

    else if (g_strcmp0(key, "terminal") == 0)
        self->terminal = g_settings_get_string(settings, key);

    else if (g_strcmp0(key, "list-view-size-units") == 0)
        self->list_view_size_units = g_settings_get_string(settings, key);

    else if (g_strcmp0(key, "saved-search") == 0)
        self->saved_search = g_settings_get_string(settings, key);

    else if (g_strcmp0(key, "format-command") == 0)
        self->format_cmd = g_settings_get_string(settings, key);

    else if (g_strcmp0(key, "modules-blacklist") == 0)
        self->modules_blacklist = g_settings_get_strv(settings, key);

    else if (g_strcmp0(key, "modules-whitelist") == 0)
        self->modules_whitelist = g_settings_get_strv(settings, key);

    else if (g_strcmp0(key, "si-unit") == 0)
        self->si_unit = g_settings_get_boolean(settings, key);

    else if (g_strcmp0(key, "single-click") == 0)
        self->single_click = g_settings_get_boolean(settings, key);

    else if (g_strcmp0(key, "use-trash") == 0)
        self->use_trash = g_settings_get_boolean(settings, key);

    else if (g_strcmp0(key, "confirm-trash") == 0)
        self->confirm_trash = g_settings_get_boolean(settings, key);

    else if (g_strcmp0(key, "confirm-deletion") == 0)
        self->confirm_del = g_settings_get_boolean(settings, key);

    else if (g_strcmp0(key, "show-thumbnails") == 0)
        self->show_thumbnail = g_settings_get_boolean(settings, key); 

    else if (g_strcmp0(key, "thumbnail-only-local") == 0)
        self->thumbnail_local = g_settings_get_boolean(settings, key); 

    else if (g_strcmp0(key, "startup-notify") == 0)
        self->force_startup_notify = g_settings_get_boolean(settings, key); 

    else if (g_strcmp0(key, "backup-hidden") == 0)
        self->backup_as_hidden = g_settings_get_boolean(settings, key); 

    else if (g_strcmp0(key, "no-usb-trash") == 0)
        self->no_usb_trash = g_settings_get_boolean(settings, key);         

    else if (g_strcmp0(key, "expand-empty") == 0)
        self->no_child_non_expandable = g_settings_get_boolean(settings, key);    

    else if (g_strcmp0(key, "show-full-names") == 0)
        self->show_full_names = g_settings_get_boolean(settings, key);    

    else if (g_strcmp0(key, "only-user-templates") == 0)
        self->only_user_templates = g_settings_get_boolean(settings, key);    

    else if (g_strcmp0(key, "template-run-app") == 0)
        self->template_run_app = g_settings_get_boolean(settings, key);            

    else if (g_strcmp0(key, "template-type-once") == 0)
        self->template_type_once = g_settings_get_boolean(settings, key);    

    else if (g_strcmp0(key, "advanced-mode") == 0)
        self->advanced_mode = g_settings_get_boolean(settings, key);    

    else if (g_strcmp0(key, "shadow-hidden") == 0)
        self->shadow_hidden = g_settings_get_boolean(settings, key);    

    else if (g_strcmp0(key, "defer-content-test") == 0)
        self->defer_content_test = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "quick-exec") == 0)
        self->quick_exec = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "smart-desktop-drop") == 0)
        self->smart_desktop_autodrop = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-home") == 0)
        self->places_home = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-desktop") == 0)
        self->places_desktop = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-root") == 0)
        self->places_root = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-computer") == 0)
        self->places_computer = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-trash") == 0)
        self->places_trash = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-applications") == 0)
        self->places_applications = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-network") == 0)
        self->places_network = g_settings_get_boolean(settings, key);  

    else if (g_strcmp0(key, "show-places-unmounted") == 0)
        self->places_unmounted = g_settings_get_boolean(settings, key); 

    else if (g_strcmp0(key, "auto-selection-delay") == 0)
        self->auto_selection_delay = g_settings_get_int(settings, key); 

    else if (g_strcmp0(key, "thumbnail-maximum-size") == 0)
        self->thumbnail_max = g_settings_get_int(settings, key); 

    else if (g_strcmp0(key, "big-icon-size") == 0)
        self->big_icon_size = g_settings_get_int(settings, key); 

    else if (g_strcmp0(key, "small-icon-size") == 0)
        self->small_icon_size = g_settings_get_int(settings, key); 

    else if (g_strcmp0(key, "thumbnail-icon-size") == 0)
        self->thumbnail_size = g_settings_get_int(settings, key); 

    else if (g_strcmp0(key, "pane-icon-size") == 0)
        self->pane_icon_size = g_settings_get_int(settings, key); 

    fm_config_emit_changed(self, (const char*) key);
}

static void fm_config_class_init(FmConfigClass *klass)
{
    GObjectClass *g_object_class;

    g_object_class = G_OBJECT_CLASS(klass);
    g_object_class->finalize = fm_config_finalize;

    /**
     * FmConfig::changed:
     * @config: configuration that was changed
     *
     * The #FmConfig::changed signal is emitted when a config key is changed.
     *
     * Since: 0.1.0
     */
    signals[CHANGED]=
        g_signal_new("changed",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_FIRST|G_SIGNAL_DETAILED,
                     G_STRUCT_OFFSET(FmConfigClass, changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);

}

static void fm_config_finalize(GObject *object)
{
    FmConfig* cfg;
    g_return_if_fail(object != NULL);
    g_return_if_fail(FM_IS_CONFIG(object));

    cfg = (FmConfig*)object;
    g_settings_sync();

    if(cfg->terminal)
        g_free(cfg->terminal);
    if(cfg->archiver)
        g_free(cfg->archiver);
    g_strfreev(cfg->system_modules_blacklist);
    g_strfreev(cfg->modules_blacklist);
    g_strfreev(cfg->modules_whitelist);
    g_free(cfg->format_cmd);
    g_free(cfg->list_view_size_units);
    g_free(cfg->saved_search);

    G_OBJECT_CLASS(fm_config_parent_class)->finalize(object);
}


static void fm_config_init(FmConfig *self)
{
    /** 
     * default settings profile
     */
    self->settings = g_settings_new_with_path(
        "com.github.libcephalopod", "/com/github/libcephalopod/default/");
  
    g_signal_connect(self->settings, "changed",
                     G_CALLBACK(on_external_settings_change), self);
    /**
     * Since we use a relocatable path to support multiple profiles
     * the settings path is invisible in dconf-editor because nothing has
     * changed and gsettings only stores settings that are different 
     * from the default, so dconf-editor is not aware of the path.
     * We just set some value to it's current settings to make
     * dconf-editor see the path.
     * Alternativetely one could add the path to dconf-editor's 
     * own settings. TODO: contact dconf-editor mainainer
     * to add /com/github/libcephalopod/* and /com/github/cephalopod/*
     * Than this workaround can be dropped
     */
    gboolean uxplus = g_settings_get_boolean(self->settings, "si-unit");
    g_settings_set_boolean(self->settings, "si-unit", uxplus);        
}

/**
 * fm_config_new
 *
 * Creates a new configuration structure filled with default values.
 *
 * Return value: a new #FmConfig object.
 *
 * Since: 0.1.0
 */
FmConfig *fm_config_new(void)
{
    return (FmConfig*)g_object_new(FM_CONFIG_TYPE, NULL);
}

/**
 * fm_config_emit_changed
 * @cfg: pointer to configuration
 * @changed_key: what was changed
 *
 * Causes the #FmConfig::changed signal to be emitted.
 *
 * This API is not thread-safe and should be used only in default context.
 */
void fm_config_emit_changed(FmConfig* cfg, const char* changed_key)
{
    GQuark detail = changed_key ? g_quark_from_string(changed_key) : 0;
    g_signal_emit(cfg, signals[CHANGED], detail);
}

/**
 * fm_config_reset
 * @cfg: pointer to configuration
 * @key: what should be reset
 */
void fm_config_reset(FmConfig *cfg, const char *key)
{
   g_settings_reset (cfg->settings, key);
}

/**
 * fm_config_load
 * @cfg: pointer to configuration
 * Fills configuration @cfg with data gsettings
 */
void fm_config_load(FmConfig* cfg)
{
    char **strv;
    cfg->use_trash = g_settings_get_boolean(cfg->settings, "use-trash");
    cfg->single_click = g_settings_get_boolean(cfg->settings, "single-click");
    cfg->confirm_del = g_settings_get_boolean(cfg->settings, "confirm-deletion");
    cfg->confirm_trash = g_settings_get_boolean(cfg->settings, "confirm-trash");
    cfg->thumbnail_local = g_settings_get_boolean(cfg->settings, "thumbnail-only-local");
    cfg->advanced_mode = g_settings_get_boolean(cfg->settings, "advanced-mode");
    cfg->si_unit = g_settings_get_boolean(cfg->settings, "si-unit");
    cfg->force_startup_notify = g_settings_get_boolean(cfg->settings, "startup-notify");
    cfg->backup_as_hidden = g_settings_get_boolean(cfg->settings, "backup-hidden");
    cfg->no_usb_trash = g_settings_get_boolean(cfg->settings, "no-usb-trash");
    cfg->no_child_non_expandable = g_settings_get_boolean(cfg->settings, "expand-empty");
    cfg->show_full_names = g_settings_get_boolean(cfg->settings, "show-full-names");
    cfg->only_user_templates = g_settings_get_boolean(cfg->settings, "only-user-templates");
    cfg->template_run_app = g_settings_get_boolean(cfg->settings, "template-run-app");
    cfg->template_type_once = g_settings_get_boolean(cfg->settings, "template-type-once");
    cfg->defer_content_test = g_settings_get_boolean(cfg->settings, "defer-content-test");
    cfg->quick_exec = g_settings_get_boolean(cfg->settings, "quick-exec");
    cfg->smart_desktop_autodrop = g_settings_get_boolean(cfg->settings, "smart-desktop-drop");
    cfg->places_home = g_settings_get_boolean(cfg->settings, "show-places-home");
    cfg->places_desktop = g_settings_get_boolean(cfg->settings, "show-places-desktop");
    cfg->places_root = g_settings_get_boolean(cfg->settings, "show-places-root");
    cfg->places_computer = g_settings_get_boolean(cfg->settings, "show-places-computer");
    cfg->places_trash = g_settings_get_boolean(cfg->settings, "show-places-trash");
    cfg->places_applications = g_settings_get_boolean(cfg->settings, "show-places-applications");
    cfg->places_network = g_settings_get_boolean(cfg->settings, "show-places-network");
    cfg->places_unmounted = g_settings_get_boolean(cfg->settings, "show-places-unmounted");
    cfg->show_thumbnail = g_settings_get_boolean(cfg->settings, "show-thumbnails");
    cfg->shadow_hidden = g_settings_get_boolean(cfg->settings, "shadow-hidden");   
    cfg->thumbnail_max = g_settings_get_int(cfg->settings, "thumbnail-maximum-size");
    cfg->big_icon_size = g_settings_get_int(cfg->settings, "big-icon-size");
    cfg->small_icon_size = g_settings_get_int(cfg->settings, "small-icon-size");
    cfg->pane_icon_size = g_settings_get_int(cfg->settings, "pane-icon-size");
    cfg->thumbnail_size = g_settings_get_int(cfg->settings, "thumbnail-icon-size");
    cfg->auto_selection_delay = g_settings_get_int(cfg->settings, "auto-selection-delay");

    g_free(cfg->list_view_size_units);
    cfg->list_view_size_units = g_settings_get_string(cfg->settings, "list-view-size-units");
  
    g_free(cfg->saved_search);
    cfg->saved_search = g_settings_get_string(cfg->settings, "saved-search");

    g_free(cfg->format_cmd);
    cfg->format_cmd = g_settings_get_string(cfg->settings, "format-command");

    if(cfg->terminal)
        g_free(cfg->terminal);
    cfg->terminal = g_settings_get_string(cfg->settings, "terminal");

    if(cfg->archiver)
        g_free(cfg->archiver);
    cfg->archiver = g_settings_get_string(cfg->settings, "archiver");

    cfg->drop_default_action = drop_action_from_str(cfg);     
 
    /* append blacklist */
    strv = g_settings_get_strv(cfg->settings, "modules-blacklist");
    fm_strcatv(&cfg->modules_blacklist, strv);
    g_strfreev(strv);

    /* replace whitelist */
    g_strfreev(cfg->modules_whitelist);
    cfg->modules_whitelist = g_settings_get_strv(cfg->settings, "modules-whitelist");

    g_signal_emit(cfg, signals[CHANGED], 0);
}

/**
 * fm_config_save
 * @cfg: pointer to configuration
 *
 * Sync config to gsettings because of user interaction
 * with preference windows 
 * TODO: It shouldn't be required to set *all* settings
 * when only 1 may changed.
 * TODO: g_settings_set_* can return false in case 
 * of admin overrides. Show message indicating 
 * that this setting is not allowed.
 */
void fm_config_save(FmConfig* cfg)
{
  g_settings_set_boolean (cfg->settings, "use-trash", cfg->use_trash);   
  g_settings_set_boolean (cfg->settings, "single-click", cfg->single_click);
  g_settings_set_boolean (cfg->settings, "confirm-deletion", cfg->confirm_del);
  g_settings_set_boolean (cfg->settings, "confirm-trash", cfg->confirm_trash);
  g_settings_set_boolean (cfg->settings, "thumbnail-only-local", cfg->thumbnail_local);
  g_settings_set_boolean (cfg->settings, "advanced-mode", cfg->advanced_mode);
  g_settings_set_boolean (cfg->settings, "si-unit", cfg->si_unit);
  g_settings_set_boolean (cfg->settings, "startup-notify", cfg->force_startup_notify);
  g_settings_set_boolean (cfg->settings, "backup-hidden", cfg->backup_as_hidden);
  g_settings_set_boolean (cfg->settings, "no-usb-trash", cfg->no_usb_trash);
  g_settings_set_boolean (cfg->settings, "expand-empty", cfg->no_child_non_expandable);
  g_settings_set_boolean (cfg->settings, "show-full-names", cfg->show_full_names);
  g_settings_set_boolean (cfg->settings, "only-user-templates", cfg->only_user_templates);
  g_settings_set_boolean (cfg->settings, "template-run-app", cfg->template_run_app);
  g_settings_set_boolean (cfg->settings, "template-type-once", cfg->template_type_once);
  g_settings_set_boolean (cfg->settings, "defer-content-test", cfg->defer_content_test);
  g_settings_set_boolean (cfg->settings, "quick-exec", cfg->quick_exec);
  g_settings_set_boolean (cfg->settings, "smart-desktop-drop", cfg->smart_desktop_autodrop);
  g_settings_set_boolean (cfg->settings, "show-places-home", cfg->places_home);
  g_settings_set_boolean (cfg->settings, "show-places-desktop", cfg->places_desktop);
  g_settings_set_boolean (cfg->settings, "show-places-root", cfg->places_root);
  g_settings_set_boolean (cfg->settings, "show-places-computer", cfg->places_computer);
  g_settings_set_boolean (cfg->settings, "show-places-trash", cfg->places_trash);
  g_settings_set_boolean (cfg->settings, "show-places-applications", cfg->places_applications);
  g_settings_set_boolean (cfg->settings, "show-places-network", cfg->places_network);
  g_settings_set_boolean (cfg->settings, "show-places-unmounted", cfg->places_unmounted);
  g_settings_set_boolean (cfg->settings, "show-thumbnails", cfg->show_thumbnail);
  g_settings_set_boolean (cfg->settings, "shadow-hidden", cfg->shadow_hidden);

  g_settings_set_int (cfg->settings, "thumbnail-maximum-size", cfg->thumbnail_max);
  g_settings_set_int (cfg->settings, "big-icon-size", cfg->big_icon_size);
  g_settings_set_int (cfg->settings, "small-icon-size", cfg->small_icon_size);
  g_settings_set_int (cfg->settings, "pane-icon-size", cfg->pane_icon_size);
  g_settings_set_int (cfg->settings, "thumbnail-icon-size", cfg->thumbnail_size);
  g_settings_set_int (cfg->settings, "auto-selection-delay", cfg->auto_selection_delay);

  g_settings_set_string (cfg->settings, "terminal", cfg->terminal);
  g_settings_set_string (cfg->settings, "archiver", cfg->archiver);
  g_settings_set_string (cfg->settings, "format-command", cfg->format_cmd);
  g_settings_set_string (cfg->settings, "list-view-size-units", cfg->list_view_size_units);
  g_settings_set_string (cfg->settings, "saved-search", cfg->saved_search);

  g_settings_set_strv (cfg->settings, "modules-blacklist", cfg->modules_blacklist);
  g_settings_set_strv (cfg->settings, "modules-whitelist", cfg->modules_whitelist);

  if (cfg->list_view_size_units && cfg->list_view_size_units[0])
  cfg->list_view_size_units[1] = '\0'; /* leave only 1 char */

  if (cfg->drop_default_action = FM_DND_DEST_DROP_AUTO)
      g_settings_set_string (cfg->settings, "drop-action", "Auto");
  else if (cfg->drop_default_action = FM_DND_DEST_DROP_COPY)
      cfg->drop_default_action = FM_DND_DEST_DROP_COPY;
  else if (cfg->drop_default_action = FM_DND_DEST_DROP_MOVE)
      cfg->drop_default_action = FM_DND_DEST_DROP_MOVE;
  else if (cfg->drop_default_action = FM_DND_DEST_DROP_ASK)
      cfg->drop_default_action = FM_DND_DEST_DROP_ASK;   
  else
      g_error("Impossible value detected");      
}