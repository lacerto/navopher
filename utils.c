#include "utils.h"

/* Removes the extension from the filename. */
gchar* remove_ext(gchar* filename) {
    gchar* dot = g_strrstr(filename, ".");
    if (dot != NULL) {
        *dot = '\0';
    }
    return filename;
}

/* Prepares the display name for a gophermap line. */
gchar* prepare_name(gchar* name) {
    GString* retval = NULL;
    gchar** tokens = g_strsplit(name, "__", -1);
    gint i;

    for (i=0; tokens[i] != NULL; i++) {
        if (i == 0) {
            retval = g_string_new("");
            g_string_append_printf(retval, "[%s]", tokens[i]);
        } else {
            g_string_append_printf(retval, " %s", g_strdelimit(tokens[i], "_", ' '));
        }
    }

    return g_string_free(retval, FALSE);
}

/* Returns the current local date & time. */
gchar* get_local_date_time_string(void) {
    GDateTime* date_time = g_date_time_new_now_local();
    gchar* str = g_date_time_format(date_time, "%F %T %Z");
    g_date_time_unref(date_time);
    return str;
}
