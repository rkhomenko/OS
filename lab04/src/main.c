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
        return 0;
    }

    if (is_locked(fd)) {
        printf("File \"%s\" is already locked!\n", file_name);
        return 0;
    }

    lock_file(fd);

    return fd;
}

static void close_file(int fd) {
    unlock_file(fd);
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
        return 0;
    }
    result = (result < PAGE_SIZE) ? 1 : result / PAGE_SIZE;
    return result * PAGE_SIZE;
}

static const char* help_message =
    "Simple text editor for lab04. Don't use it!\n"
    "-m\tset max memory for mmap\n"
    "  \t(value will be rounded off to be multiple page size)\n"
    "-f\tset file name for operations\n"
    "-s\tsearch in file\n"
    "-S\tsearch in file with ignore case\n"
    "-h\tprint this message and exit\n";

static const char* help_message_interactive_mode =
    "m value            set max memory for mmap\n"
    "                   (value will be rounded off to be multiple page size)\n"
    "f file_name        set file name for operations\n"
    "s search_pattern   search in file with case sens\n"
    "S search_pattern   search in file with ignore case\n"
    "h                  print this message\n";

static void read_strings(char* cmd, char* arg, size_t size) {
    size_t index = 0;
    int c = 0;
    while ((c = getchar()) && c != EOF && c != ' ' && c != '\n' && index < size) {
        cmd[index] = (char)c;
        index++;
    }

    cmd[index] = '\0';

    if (c == '\n') {
        return;
    }
    else if (c == EOF) {
        cmd[size - 1] = 'E';
        return;
    }

    index = 0;
    c = 0;
    while ((c = getchar()) && c != EOF && c != '\n' && index < size) {
        if (c == ' ') {
            printf("Cannot read argument!\n");
            return;
        }
        arg[index] = (char)c;
        index++;
    }

    arg[index] = '\0';

    if (c == EOF) {
        arg[size - 1] = 'E';
    }
}

static void interactive_mode(void) {
    const char* help_cmd = "h";
    const char* quit_cmd = "q";
    const char* memory_cmd = "m";
    const char* file_cmd = "f";
    const char* search_cmd = "s";
    const char* search_ignore_case_cmd = "S";
    const size_t BUFFER_SIZE = to_memory_value(NULL);

    char cmd[BUFFER_SIZE];
    char arg[BUFFER_SIZE];
    size_t cmd_size = 0;
    size_t arg_size = 0;
    size_t memory = to_memory_value(NULL);
    int fd = 0;

    while (1) {
        printf(">> ");
        read_strings(cmd, arg, BUFFER_SIZE);

        cmd_size = strlen(cmd);
        arg_size = strlen(arg);

        if (cmd_size == 0) {
            if (cmd[BUFFER_SIZE - 1] == 'E') {
                if (fd != 0) {
                    close_file(fd);
                }
                exit(EXIT_SUCCESS);
            }
            else {
                printf("Command not set!\n");
            }
            continue;
        }

        if (strcmp(cmd, help_cmd) == 0) {
            printf("%s", help_message_interactive_mode);
            continue;
        }
        else if (strcmp(cmd, quit_cmd) == 0) {
            if (fd != 0) {
                close_file(fd);
            }
            exit(EXIT_SUCCESS);
        }
        else if (arg_size == 0) {
            if (arg[BUFFER_SIZE - 1] == 'E') {
                if (fd != 0) {
                    close_file(fd);
                }
                exit(EXIT_SUCCESS);
            }
            else {
                printf("Argument not set!\n");
            }
            continue;
        }
        else if (strcmp(cmd, memory_cmd) == 0) {
            memory = to_memory_value(arg);
        }
        else if (strcmp(cmd, file_cmd) == 0) {
            fd = open_file(arg);
        }
        else if (strcmp(cmd, search_cmd) == 0) {
            if (fd == 0) {
                printf("File not set!\n");
                continue;
            }
            find(fd, arg, FT_CASE_SENS, memory);
        }
        else if (strcmp(cmd, search_ignore_case_cmd) == 0) {
            if (fd == 0) {
                printf("File not set!\n");
                continue;
            }
            find(fd, arg, FT_CASE_IGNORE, memory);
        }
        else {
            printf("Unknown command! Try h for help.\n");
        }
    }
}

int main(int argc, char** argv) {
    int opt = 0;
    char* file_name = NULL;
    size_t memory = to_memory_value(NULL);

    if (argc == 1) {
        interactive_mode();
    }

    int fd = 0;
    while ((opt = getopt(argc, argv, "m:f:s:S:h")) != -1) {
        switch (opt) {
            case 'm':
                memory = to_memory_value(optarg);
                if (memory == 0) {
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                file_name = optarg;
                if (fd != 0) {
                    close_file(fd);
                }
                fd = open_file(file_name);
                if (fd == 0) {
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                check_fd(fd);
                find(fd, optarg, FT_CASE_SENS, memory);
                break;
            case 'S':
                check_fd(fd);
                find(fd, optarg, FT_CASE_IGNORE, memory);
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
