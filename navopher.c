#include <stdio.h>
#include <stdlib.h>
#include "gmap.h"

#define VERSION "2.0"

/*
Prints usage information.
*/
void print_usage(void);

/*
Prints the version.
*/
void print_version(void);

int main(int argc, char* argv[argc+1]) {
    GFile* dir = NULL;
    struct file_lists lists;
    GSList* template_lines = NULL;
    gboolean success;
    int ch;

    g_set_prgname(g_path_get_basename(argv[0]));

	while ((ch = getopt(argc, argv, "vh")) != -1) {
		switch (ch) {
			case 'v':
				print_version();
				return EXIT_SUCCESS;
			case 'h':
				print_usage();
				return EXIT_SUCCESS;
			default:
				print_usage();
				return EXIT_FAILURE;
		}
	}

    if (argc != 3) {
        print_usage();
        return EXIT_FAILURE;
    }

    gchar* path = g_build_filename(argv[1], argv[2], NULL);
    template_lines = read_template_file(path);
    g_free(path);

    if (template_lines == NULL) {
        return EXIT_FAILURE;
    }

    dir = g_file_new_for_commandline_arg(argv[1]);
    lists = get_file_lists(dir);
    g_object_unref(dir);

    if (lists.regular == NULL && lists.archive == NULL) {
        return EXIT_FAILURE;
    }

    success = create_gophermap(argv[1], template_lines, lists);
    
    if (lists.regular != NULL) g_slist_free_full(lists.regular, free_map_line);
    if (lists.archive != NULL) g_slist_free_full(lists.archive, free_map_line);
    g_slist_free_full(template_lines, g_free);  
    
    if (!success) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
Prints usage information.
*/
void print_usage(void) {
    gchar const* prgname = g_get_prgname();
    printf("\nGenerate a gophermap for phlog posts.\n\n");
    printf("Usage:\n  %s PHLOG_DIRECTORY GOPHERMAP_TEMPLATE\n", prgname);
    printf("  %s -v\n", prgname);
	printf("  %s -h\n", prgname);
    printf("\nRequired arguments:\n");
    printf("  PHLOG_DIRECTORY\tdirectory containing phlog posts\n");
    printf("  GOPHERMAP_TEMPLATE\tname of the template file in PHLOG_DIRECTORY\n");
    printf("\nThe placeholders in the template will be replaced with the posts list\n"
           "and the gophermap generated in the given directory.\n");
	printf("\nOptions:\n");
	printf("  -v\tShow version.\n");
	printf("  -h\tShow this help.\n\n");

}

void print_version() {
    printf("This is Navopher version %s\n", VERSION);
}