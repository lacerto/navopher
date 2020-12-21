#ifndef GMAP_H
#define GMAP_H

#include <glib.h>
#include <gio/gio.h>

#define ARCHIVE_PREFIX "AR_"

/*
Enumeration for signaling regular phlog posts (including sub-directories)
and archive directories containing the posts of previous years.
*/
enum file_type { ft_regular, ft_archive };

/*
Structure for a gophermap line:
XnameTABselector
where X is the gopher_type, TAB is the \t character.
For type see enum file_type.
*/
struct mapline {
    enum file_type type;
    gchar gopher_type;
    gchar* name;
    gchar* selector;
};

/*
Data for processing map lines.
type keeps the regular posts and the archives apart.
*/
struct foreach_param {
    enum file_type type;
    GOutputStream* stream;
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
*.glink files point to resources on another server 
(see handle_glink).

Returns a pointer to a newly allocated mapline structure.
Free the struct and its contents when you are done with them.
*/
struct mapline* handle_regular_file(GFile* dir, GFileInfo* info);

/*
Creates a gophermap line for a glink (to a file on another server).
The file's name is treated like that of a regular file
(see handle_regular_file) and the contents of the glink file are
read and used as a selector in the resulting gophermap line.

Returns a pointer to a newly allocated mapline structure.
Free the struct and its contents when you are done with them.
*/
struct mapline* handle_glink(GFile* dir, const char* file_name);

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
