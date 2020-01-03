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
gint compare_map_lines(gconstpointer a, gconstpointer b);
struct mapline* handle_regular_file(GFileInfo* info);
struct mapline* handle_directory(GFileInfo* info);
gchar* remove_ext(gchar* filename);
gchar* prepare_name(gchar* name);
void read_template_file(gchar* path);

int main(int argc, char** argv) {
    GFile* dir = NULL;
    GSList* map_lines = NULL;

    if (argc != 3) {
        return EXIT_FAILURE;
    }

    read_template_file(g_build_filename(argv[1], argv[2], NULL));

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
            fprintf(stderr, "%s\n", err->message);
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

    map_lines = g_slist_sort(map_lines, compare_map_lines);
    map_lines = g_slist_reverse(map_lines);
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

gint compare_map_lines(gconstpointer a, gconstpointer b) {
    struct mapline const* line_a = (struct mapline*) a;
    struct mapline const* line_b = (struct mapline*) b;

    return g_strcmp0(line_a->name, line_b->name);
}

struct mapline* handle_regular_file(GFileInfo* info) {
    struct mapline* line = NULL;
    gchar const* file_name = g_file_info_get_display_name(info); 

    if (g_str_has_suffix(file_name, ".txt") || g_str_has_suffix(file_name, ".md")) {
        gchar* name = g_strdup(file_name);
        line = g_new(struct mapline, 1);
        line->type = 0;
        line->name = prepare_name(remove_ext(name));
        line->selector = g_strdup(file_name);
        g_free(name);
    }

    return line;
}

struct mapline* handle_directory(GFileInfo* info) {
    struct mapline* line = NULL;
    gchar const* name = g_file_info_get_display_name(info); 

    line = g_new(struct mapline, 1);
    line->type = 1;
    line->name = prepare_name(g_strdup(name));
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

void read_template_file(gchar* path) {
    GFile* template_file = NULL;
    GFileInputStream* in_stream = NULL;
    GDataInputStream* data_stream = NULL;
    GError* err = NULL;
    gchar* line;

    template_file = g_file_new_for_path(path);
    in_stream = g_file_read(template_file, NULL, &err);
    if (err != NULL) {
        fprintf(stderr, "%s\n", err->message);
        g_error_free(err);
        g_object_unref(template_file);
        return;
    }

    data_stream = g_data_input_stream_new((GInputStream*) in_stream);
    while (TRUE) {
        line = g_data_input_stream_read_line_utf8(data_stream, NULL, NULL, &err);
        if (err != NULL) {
            fprintf(stderr, "%s\n", err->message);
            g_error_free(err);
            break;
        }
        // EOF reached
        if (line == NULL) break;

        printf("%s\n", line);
        g_free(line);
    }

    g_object_unref(data_stream);
    g_object_unref(in_stream);
    g_object_unref(template_file);
}
