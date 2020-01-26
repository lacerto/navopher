#include <stdio.h>
#include <stdlib.h>
#include "gmap.h"

/*
Prints usage information.
*/
void print_help(void);

int main(int argc, char** argv) {
    GFile* dir = NULL;
    GSList* map_lines = NULL;
    GSList* template_lines = NULL;
    gboolean success;

    g_set_prgname(g_path_get_basename(argv[0]));

    if (argc != 3) {
        print_help();
        return EXIT_FAILURE;
    }

    gchar* path = g_build_filename(argv[1], argv[2], NULL);
    template_lines = read_template_file(path);
    g_free(path);

    if (template_lines == NULL) {
        return EXIT_FAILURE;
    }

    dir = g_file_new_for_commandline_arg(argv[1]);
    map_lines = get_map_lines(dir);
    g_object_unref(dir);

    if (map_lines == NULL) {
        return EXIT_FAILURE;
    }

    success = create_gophermap(argv[1], template_lines, map_lines);
    
    g_slist_free_full(map_lines, free_map_line);
    g_slist_free_full(template_lines, g_free);  
    
    if (!success) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
Prints usage information.
*/
void print_help(void) {
    printf("Usage: %s DIRECTORY FILE\n", g_get_prgname());
    puts("Take the contents of DIRECTORY and generate a gophermap");
    puts("based on the template found in FILE.");
    puts("The generated gophermap is placed in DIRECTORY.");
}
