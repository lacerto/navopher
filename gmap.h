#ifndef GMAP_H
#define GMAP_H

#include <glib.h>
#include <gio/gio.h>

/*
Structure for a gophermap line:
XnameTABselector
where X is the type, TAB is the \t character.
*/
struct mapline {
    gchar type;
    gchar* name;
    gchar* selector;
};

/*
Reads a directory's contents and create gophermap lines for
each text file (txt, md) and each directory.
The file and directory names should have the format
YYYY-MM-DD__Phlog_Entry_Title[.txt|.md]

Returns a pointer to a GSList.
Free the list when you are done with it.
*/
GSList* get_map_lines(GFile* dir);

/*
Creates a gophermap line (fill the structure) for a
regular (text) file. Only *.md and *.txt files are
processed.

Returns a pointer to a newly allocated mapline structure.
Free the struct and its contents when you are done with them.
*/
struct mapline* handle_regular_file(GFileInfo* info);

/*
Creates a gophermap line (fill the structure) for a
directory.

Returns a pointer to a newly allocated mapline structure.
Free the struct and its contents when you are done with them.
*/
struct mapline* handle_directory(GFileInfo* info);

/*
Frees a mapline structure and the strings its
members point to.
*/
void free_map_line(gpointer item);

/*
Compares two mapline structures. Only the name members
are compared as strings. 
*/
gint compare_map_lines(gconstpointer a, gconstpointer b);

/*
Reads the template file and puts each line in a
GSList.

Returns a pointer to a GSList.
Free the list when you are done with it.
*/
GSList* read_template_file(gchar* path);

/*
Creates a gophermap file in the given directory.
Special keywords in the template file are substituted with 
the list of map lines created based on the directories and text files
in the directory or with the current date and time.

If the gophermap exists it is simply overwritten.

Returns TRUE if no error occurred.
*/
gboolean create_gophermap(gchar* dir, GSList* template_lines, GSList* map_lines);

/*
Processes a single template line.
Substitutes the {{ FILE_LIST }} and {{ DATE_TIME }} keywords.

Returns TRUE if no error occurred.
*/
gboolean process_line(gchar const* line, GSList* map_lines, GOutputStream* out_stream);

/*
Writes the contents of the mapline structure to an
output stream.
*/
void write_map_line(gpointer item, gpointer stream);

#endif