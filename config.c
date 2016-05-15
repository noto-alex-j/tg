/*
    tg
    Copyright (C) 2015 Marcello Mamino

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "tg.h"

#define g_key_file_get_int g_key_file_get_integer
#define g_key_file_set_int g_key_file_set_integer // the devil may take gtk

#define FIELDS(OP) \
	OP(bph, bph, int) \
	OP(lift_angle, la, double) \
	OP(calibration, cal, double)

void load_config(struct main_window *w)
{
	w->config_file = g_key_file_new();
	w->config_file_name = g_build_filename(g_get_user_config_dir(), CONFIG_FILE_NAME, NULL);
	debug("Config: loading configuration file %s\n", w->config_file_name);
	gboolean ret = g_key_file_load_from_file(w->config_file, w->config_file_name, G_KEY_FILE_KEEP_COMMENTS, NULL);
	if(!ret) {
		debug("Config: failed to load config file");
		return;
	}

#define LOAD(NAME,PLACE,TYPE) \
	{ \
		GError *e = NULL; \
		TYPE val = g_key_file_get_ ## TYPE (w->config_file, "tg", #NAME, &e); \
		if(e) { \
			debug("Config: error loading field " #NAME "\n"); \
			g_error_free(e); \
		} else \
			w -> PLACE = val; \
	}

	FIELDS(LOAD);
}

void save_config(struct main_window *w)
{
	debug("Config: saving configuration file\n");

	g_key_file_set_string(w->config_file, "tg", "version", VERSION);

#define SAVE(NAME,PLACE,TYPE) \
	g_key_file_set_ ## TYPE (w->config_file, "tg", #NAME, w -> PLACE);

	FIELDS(SAVE);

#ifdef DEBUG
	gboolean ret =
#endif
	g_key_file_save_to_file(w->config_file, w->config_file_name, NULL);

#ifdef DEBUG
	if(!ret) {
		debug("Config: failed to save config file");
		if(testing) exit(1);
	}
#endif
}