#include <stdio.h>
#include "gmap.h"
#include "utils.h"

/* Reads a directory's contents and creates gophermap lines. */
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

/* Creates a gophermap line for a regular (text) file. */
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

/* Creates a gophermap line for a directory. */
struct mapline* handle_directory(GFileInfo* info) {
    struct mapline* line = NULL;
    gchar const* name = g_file_info_get_display_name(info); 

    line = g_new(struct mapline, 1);
    line->type = 1;
    line->name = prepare_name(g_strdup(name));
    line->selector = g_strdup(name);

    return line;
}

/* Frees a mapline structure and the strings its members point to. */
void free_map_line(gpointer item) {
    struct mapline* line = (struct mapline*) item;
    g_free(line->name);
    g_free(line->selector);
    g_free(line);
}

/* Compares two mapline structures. */
gint compare_map_lines(gconstpointer a, gconstpointer b) {
    struct mapline const* line_a = (struct mapline*) a;
    struct mapline const* line_b = (struct mapline*) b;

    return g_strcmp0(line_a->name, line_b->name);
}

/* Reads the template file. */
GSList* read_template_file(gchar* path) {
    GFile* template_file = NULL;
    GFileInputStream* in_stream = NULL;
    GDataInputStream* data_stream = NULL;
    GError* err = NULL;
    GSList* template_lines = NULL;
    gchar* line;

    template_file = g_file_new_for_path(path);
    in_stream = g_file_read(template_file, NULL, &err);
    if (err != NULL) {
        fprintf(stderr, "%s\n", err->message);
        g_error_free(err);
        g_object_unref(template_file);
        return NULL;
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

        // Add the line to the list. Lines must be freed afterwards.
        template_lines = g_slist_append(template_lines, line);
    }

    g_object_unref(data_stream);
    g_object_unref(in_stream);
    g_object_unref(template_file);
    return template_lines;
}

/* Creates a gophermap file in the given directory. */
gboolean create_gophermap(gchar* dir, GSList* template_lines, GSList* map_lines) {
    GFile* gophermap = NULL;
    GFileOutputStream* out_stream = NULL;
    GError* err = NULL;

    gchar* path = g_build_filename(dir, "gophermap", NULL);
    gophermap = g_file_new_for_path(path);
    g_free(path);
    out_stream = g_file_replace(
        gophermap,
        NULL,
        FALSE,
        G_FILE_CREATE_NONE,
        NULL,
        &err
    );
    if (err != NULL) {
        fprintf(stderr, "%s\n", err->message);
        g_error_free(err);
        g_object_unref(gophermap);
        return FALSE;
    }

    for (GSList* it = template_lines; it; it = it->next) {
        gboolean success = process_line(
            (gchar*) it->data,
            map_lines,
            (GOutputStream*) out_stream);
        if (!success) break;
    }

    g_object_unref(out_stream);
    g_object_unref(gophermap);
    return TRUE;
}

/* Processes a single template line. */
gboolean process_line(gchar const* line, GSList* map_lines, GOutputStream* out_stream) {
    GError* err = NULL;
    gboolean success = TRUE;
    gboolean write_orig_line = TRUE;
    gchar const* const FILE_LIST = "FILE_LIST";
    gchar const* const DATE_TIME = "DATE_TIME";

    gchar* start = g_strstr_len(line, -1, "{{");
    if (start) {
        gchar* end = g_strrstr(line, "}}");
        if (end) {
            gchar* keyword = g_strndup(start+2, (end-start-2));
            g_strstrip(keyword);
            if (g_strcmp0(keyword, FILE_LIST) == 0) {
                g_slist_foreach(map_lines, write_map_line, out_stream);
                write_orig_line = FALSE;
            } else if (g_strcmp0(keyword, DATE_TIME) == 0) {
                GString* str = g_string_new_len(line, start-line);
                gchar* date_time = get_local_date_time_string();
                str = g_string_append(str, date_time);
                g_free(date_time);
                str = g_string_append(str, end+2);
                write_orig_line = FALSE;

                g_output_stream_printf(
                    (GOutputStream*) out_stream,
                    NULL,
                    NULL,
                    &err,
                    "%s\n",
                    str->str       
                );
                if (err != NULL) {
                    fprintf(stderr, "%s\n", err->message);
                    g_error_free(err);
                    success = FALSE;
                }
                g_string_free(str, TRUE);
            }
            g_free(keyword);
        }
    }

    if (write_orig_line) {
        g_output_stream_printf(
            (GOutputStream*) out_stream,
            NULL,
            NULL,
            &err,
            "%s\n",
            line       
        );
        if (err != NULL) {
            fprintf(stderr, "%s\n", err->message);
            g_error_free(err);
            success = FALSE;
        }
    }

    return success;
}

/* Writes a single gophermap line to an output stream. */
void write_map_line(gpointer item, gpointer stream) {
    struct mapline* line = (struct mapline*) item;
    GError* err = NULL;

    g_output_stream_printf(
        (GOutputStream*) stream,
        NULL,
        NULL,
        &err,
        "%d%s\t%s\n",
        line->type,
        line->name,
        line->selector        
    );
    if (err != NULL) {
        fprintf(stderr, "%s\n", err->message);
        g_error_free(err);
    }
}