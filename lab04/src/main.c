#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <text_editor.h>

static void interactive_mode(void) {}

static int open_file_for_reading(const char* file_name) {
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        printf("Cannot open file \"%s\": %s\n", file_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}

static size_t to_memory_value(const char* str) {
    const size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);
    size_t result = 0;

    if (str == NULL) {
        return PAGE_SIZE;
    }

    result = strtoull(str, NULL, 10);
    if (errno == ERANGE) {
        printf("Bad memory value!\n");
        exit(EXIT_FAILURE);
    }
    result = (result < PAGE_SIZE) ? 1 : result / PAGE_SIZE;
    return result * PAGE_SIZE;
}

const char* help_message =
    "Simple text editor for lab04. Don't use it!\n"
    "-m\tset max memory for mmap\n"
    "  \t(value will be rounded off to be multiple page size)\n"
    "-f\tset file name for operations\n"
    "-s\tsearch in file\n"
    "-S\tsearch in file with ignore case\n"
    "-h\tprint this message and exit\n";

int main(int argc, char** argv) {
    int opt = 0;
    char* file_name = NULL;
    size_t memory = to_memory_value(NULL);

    if (argc == 1) {
        interactive_mode();
    }

    while ((opt = getopt(argc, argv, "m:f:s:S:h")) != -1) {
        int fd = 0;
        switch (opt) {
            case 'm':
                memory = to_memory_value(optarg);
                break;
            case 'f':
                file_name = optarg;
                break;
            case 's':
                fd = open_file_for_reading(file_name);
                find(fd, optarg, FT_CASE_SENS, memory);
                close(fd);
                break;
            case 'S':
                fd = open_file_for_reading(file_name);
                find(fd, optarg, FT_CASE_IGNORE, memory);
                close(fd);
                break;
            case 'h':
                printf(help_message);
                exit(EXIT_SUCCESS);
                break;
        }
    }
}
