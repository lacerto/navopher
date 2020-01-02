#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

GSList* get_file_list(GFile* dir);
void free_file_list_item(gpointer item);
void print_file_list_item(gpointer item, gpointer user_data);

int main(int argc, char** argv) {
    GFile* dir = NULL;
    GSList* file_list = NULL;

    if (argc != 2) {
        return EXIT_FAILURE;
    }

    dir = g_file_new_for_commandline_arg(argv[1]);
    file_list = get_file_list(dir);
    g_object_unref(dir);

    if (file_list == NULL) {
        return EXIT_FAILURE;
    }

    g_slist_foreach(file_list, print_file_list_item, NULL);
    g_slist_free_full(file_list, free_file_list_item);

    return EXIT_SUCCESS;
}

GSList* get_file_list(GFile* dir) {
    GFileEnumerator* file_enumerator = NULL;
    GSList* file_list = NULL;
    GError* err = NULL;

    if (dir == NULL) return NULL;

    file_enumerator = g_file_enumerate_children(
        dir,
        G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
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

        // append item to the list
        file_list = g_slist_append(
            file_list,
            g_strdup(g_file_info_get_display_name(info))
        );
    }

    g_object_unref(file_enumerator);
    return file_list;
}

void free_file_list_item(gpointer item) {
    g_free(item);
}

void print_file_list_item(gpointer item, gpointer user_data) {
    printf("%s\n", (gchar*) item);
}
