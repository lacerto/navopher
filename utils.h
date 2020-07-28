#ifndef UTILS_H
#define UTILS_H

#include <glib.h>

/*
Removes the extension from the filename.
Returns the original filename pointer.
*/
gchar* remove_ext(gchar* filename);

/*
Prepares the display name for a gophermap line.
Tries to convert the file name:
YYYY-MM-DD__Phlog_Title -> [YYYY-MM-DD] Phlog Title

Returns a pointer to a newly allocated string, free it
when you are done with it.
*/
gchar* prepare_name(gchar* name);

/*
Returns a newly allocated string with the current date
and time in the following format:
YYYY-MM-DD HH:MM:SS TZ
*/
gchar* get_local_date_time_string(void);

#endif