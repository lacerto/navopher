#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

struct mapline {
    gchar type;
    gchar* name;
    gchar* selector;
};

GSList* get_map_lines(GFile* dir);
void free_map_line(gpointer item);
void print_map_line(gpointer item, gpointer user_data);
struct mapline* handle_regular_file(GFileInfo* info);
struct mapline* handle_directory(GFileInfo* info);
gchar* remove_ext(gchar* filename);

int main(int argc, char** argv) {
    GFile* dir = NULL;
    GSList* map_lines = NULL;

    if (argc != 2) {
        return EXIT_FAILURE;
    }

    dir = g_file_new_for_commandline_arg(argv[1]);
    map_lines = get_map_lines(dir);
    g_object_unref(dir);

    if (map_lines == NULL) {
        return EXIT_FAILURE;
    }

    g_slist_foreach(map_lines, print_map_line, NULL);
    g_slist_free_full(map_lines, free_map_line);

    return EXIT_SUCCESS;
}

GSList* get_map_lines(GFile* dir) {
    GFileEnumerator* file_enumerator = NULL;
    GSList* map_lines = NULL;
    GError* err = NULL;

    if (dir == NULL) return NULL;

    file_enumerator = g_file_enumerate_children(
        dir,
        G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME
        ","
        G_FILE_ATTRIBUTE_STANDARD_TYPE,
        G_FILE_QUERY_INFO_NONE,
        NULL,
        &err
    );
    if (err != NULL) {
        fprintf(stderr, "%s\n", err->message);
        g_error_free(err);
        return NULL;
    }

    while (TRUE) {
        gboolean success;
        GFileInfo* info;
        GFileType type;
        struct mapline* line = NULL;

        success = g_file_enumerator_iterate(
            file_enumerator,
            &info,
            NULL,
            NULL,
            &err
        );
        if (!success) {
            fprintf(stderr, "Error: %s\n", err->message);
            g_error_free(err);
            g_object_unref(file_enumerator);
            return NULL;
        }
        // end of iteration
        if (info == NULL) break;

        type = g_file_info_get_file_type(info);
        switch (type) {
            case G_FILE_TYPE_REGULAR:
                line = handle_regular_file(info);
                break;
            case G_FILE_TYPE_DIRECTORY:
                line = handle_directory(info);
                break;
            default:
                break;
        }

        // append line to the list
        if (line != NULL) {
            map_lines = g_slist_append(
                map_lines,
                line
            );
        }
    }

    g_object_unref(file_enumerator);
    return map_lines;
}

void free_map_line(gpointer item) {
    struct mapline* line = (struct mapline*) item;
    g_free(line->name);
    g_free(line->selector);
    g_free(line);
}

void print_map_line(gpointer item, gpointer user_data) {
    struct mapline* line = (struct mapline*) item;
    printf(
        "%d%s\t%s\n",
        line->type,
        line->name,
        line->selector    
    );
}

struct mapline* handle_regular_file(GFileInfo* info) {
    struct mapline* line = NULL;
    gchar const* name = g_file_info_get_display_name(info); 

    if (g_str_has_suffix(name, ".txt") || g_str_has_suffix(name, ".md")) {
        line = g_new(struct mapline, 1);
        line->type = 0;
        line->name = remove_ext(g_strdup(name));
        line->selector = g_strdup(name);
    }

    return line;
}

struct mapline* handle_directory(GFileInfo* info) {
    struct mapline* line = NULL;
    gchar const* name = g_file_info_get_display_name(info); 

    line = g_new(struct mapline, 1);
    line->type = 1;
    line->name = g_strdup(name);
    line->selector = g_strdup(name);

    return line;
}

gchar* remove_ext(gchar* filename) {
    gchar* dot = g_strrstr(filename, ".");
    if (dot != NULL) {
        *dot = '\0';
    }
    return filename;
}
