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

static int is_locked(int fd) {
    int result = 0;
    struct flock fl;

    memset(&fl, 0, sizeof(fl));

    if (fcntl(fd, F_GETLK, &fl) == -1) {
        perror("Cannot check file lock");
        exit(EXIT_FAILURE);
    }

    if (fl.l_type != F_UNLCK) {
        result = 1;
    }

    return result;
}

static void lock_file(int fd) {
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = get_file_size(fd);

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("Cannot lock file");
        exit(EXIT_FAILURE);
    }
}

static void unlock_file(int fd) {
    struct flock fl;

    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = get_file_size(fd);

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("Cannot unlock file");
        exit(EXIT_FAILURE);
    }
}

static int open_file(const char* file_name) {
    int fd = open(file_name, O_RDWR);
    if (fd < 0) {
        printf("Cannot open file \"%s\": %s\n", file_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (is_locked(fd)) {
        printf("File \"%s\" is already locked!\n", file_name);
        exit(EXIT_FAILURE);
    }

    lock_file(fd);

    return fd;
}

static void close_file(int fd) {
    if (is_locked(fd)) {
        unlock_file(fd);
    }

    close(fd);
}

static void check_fd(int fd) {
    if (fd == 0) {
        printf("File not set!\n");
        exit(EXIT_FAILURE);
    }
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

    int fd = 0;
    while ((opt = getopt(argc, argv, "m:f:s:S:h")) != -1) {
        find_type ft = FT_CASE_IGNORE;
        switch (opt) {
            case 'm':
                memory = to_memory_value(optarg);
                break;
            case 'f':
                file_name = optarg;
                if (fd != 0) {
                    close_file(fd);
                }
                fd = open_file(file_name);
                break;
            case 's':
                ft = FT_CASE_SENS;
            case 'S':
                check_fd(fd);
                find(fd, optarg, ft, memory);
                break;
            case 'h':
                printf(help_message);
                exit(EXIT_SUCCESS);
                break;
        }
    }

    if (fd != 0) {
        close_file(fd);
    }
}
