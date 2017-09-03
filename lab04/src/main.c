#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
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

static open_mode open_file(int* fd, const char* file_name) {
    open_mode mode = OM_EXIST;
    *fd = open(file_name, O_RDWR);
    if (*fd < 0) {
        *fd = 0;
        if (errno != ENOENT) {
            printf("Cannot open file \"%s\": %s\n", file_name, strerror(errno));
            return mode;
        }

        mode = OM_CREAT;
        *fd = open(file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (*fd < 0) {
            *fd = 0;
            printf("Cannot create file \"%s\": %s\n", file_name, strerror(errno));
            return mode;
        }
    }


    if (is_locked(*fd)) {
        printf("File \"%s\" is already locked!\n", file_name);
        return 0;
    }

    lock_file(*fd);

    return mode;
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

static int64_t to_int(const char* str, const char* msg) {
    size_t result = strtoll(str, NULL, 10);
    if (errno == ERANGE) {
        printf("%s\n", msg);
        return -1;
    }

    return result;
}

static size_t to_memory_value(const char* str) {
    const size_t PAGE_SIZE = sysconf(_SC_PAGESIZE);
    int64_t result = 0;

    if (str == NULL) {
        return PAGE_SIZE;
    }

    result = to_int(str, "Bad memory value!");
    result = (result < 0) ? 0 : result;
    result = (result < (int64_t)PAGE_SIZE) ? 1 : result / PAGE_SIZE;
    return result * PAGE_SIZE;
}

static int64_t get_position(int fd, const char* str) {
    const char* msg = "Bad position value!";
    const size_t SIZE = get_file_size(fd);
    int64_t pos = to_int(str, msg);

    if (pos <= 0) {
        printf("%s\n", msg);
        return pos;
    }

    if (SIZE == 0) {
        printf("WARNING! File is empty. Position set at the beginig of the file!\n");
        return 1;
    }

    if (pos > (int64_t)get_file_size(fd)) {
        printf("Bad position range!\n");
        return -1;
    }

    return pos;
}

static const char* help_message =
    "Simple text editor for lab04. Don't use it!\n"
    "-m\tset max memory for mmap\n"
    "  \t(value will be rounded off to be multiple page size)\n"
    "-f\tset file name for operations\n"
    "-s\tsearch in file\n"
    "-S\tsearch in file with ignore case\n"
    "-p\tset file position\n"
    "-a\tadd text to position\n"
    "-d\tdelete n chars from position\n"
    "-h\tprint this message and exit\n";

static const char* help_message_interactive_mode =
    "m value            set max memory for mmap\n"
    "                   (value will be rounded off to be multiple page size)\n"
    "f file_name        set file name for operations\n"
    "s search_pattern   search in file with case sens\n"
    "S search_pattern   search in file with ignore case\n"
    "p position         set position in file for add and remove commands\n"
    "a                  add data from standard input\n"
    "d count            delete count chars from position\n"
    "h                  print this message\n";

static size_t read_cmd(char* buffer, size_t size) {
    int c = 0;
    size_t index = 0;

    while ((c = getchar()) && c != EOF && c != ' ' && c != '\n' && index < size) {
        buffer[index] = (char)c;
        index++;
    }

    buffer[index] = '\0';

    if (c == EOF) {
        buffer[size - 1] = 'E';
    }

    return index;
}

static size_t read_arg(char* buffer, size_t size) {
    int c = 0;
    size_t index = 0;

    while((c = getchar()) && c != EOF && c != '\n' && index < size) {
        buffer[index] = (char)c;
        index++;
    }

    buffer[index] = '\0';

    if (c == EOF) {
        buffer[size - 1] = 'E';
    }

    return index;
}

static int fd = 0;

void close_file_at_exit(void) {
    if (fd != 0) {
        close_file(fd);
    }
}

#define CHECK_FD(fd) \
    { \
        if (fd == 0) { \
            printf("File not set!\n"); \
            continue; \
        } \
    }

#define IS_EQUAL(s1, s2) \
    (strcmp(s1, s2) == 0)

static void interactive_mode(void) {
    const char* help_cmd = "h";
    const char* quit_cmd = "q";
    const char* memory_cmd = "m";
    const char* file_cmd = "f";
    const char* search_cmd = "s";
    const char* search_ignore_case_cmd = "S";
    const char* position_cmd = "p";
    const char* add_cmd = "a";
    const char* delete_cmd = "d";
    const size_t BUFFER_SIZE = to_memory_value(NULL);

    char cmd[BUFFER_SIZE];
    char arg[BUFFER_SIZE];
    size_t cmd_size = 0;
    size_t arg_size = 0;
    size_t memory = to_memory_value(NULL);
    int64_t position = -1;
    int64_t count = -1;
    open_mode mode = OM_EXIST;

    printf("Simple text editor for lab04. Don't use it!\n");
    while (1) {
        printf(">> ");
        cmd_size = read_cmd(cmd, BUFFER_SIZE);

        if (cmd_size == 0) {
            if (cmd[BUFFER_SIZE - 1] == 'E') {
                exit(EXIT_SUCCESS);
            }
            else {
                printf("Command not set!\n");
            }
            continue;
        }

        if (IS_EQUAL(cmd, help_cmd)) {
            printf("%s", help_message_interactive_mode);
            continue;
        }
        else if (IS_EQUAL(cmd, add_cmd)) {
            CHECK_FD(fd);
            if (position < 0) {
                printf("Position not set!\n");
                continue;
            }

            add_from_stdin(fd, mode, position);
            position = -1;
        }
        else if (IS_EQUAL(cmd, quit_cmd)) {
            exit(EXIT_SUCCESS);
        }
        else {
            arg_size = read_arg(arg, BUFFER_SIZE);

            if (arg_size == 0) {
                if (arg[BUFFER_SIZE - 1] == 'E') {
                    exit(EXIT_SUCCESS);
                }
                else {
                    printf("Argument not set!\n");
                }
                continue;
            }
            else if (IS_EQUAL(cmd, memory_cmd)) {
                memory = to_memory_value(arg);
            }
            else if (IS_EQUAL(cmd, file_cmd)) {
                if (fd != 0) {
                    close_file(fd);
                }
                mode = open_file(&fd, arg);
            }
            else if (IS_EQUAL(cmd, search_cmd)) {
                CHECK_FD(fd);
                find(fd, arg, FT_CASE_SENS, memory);
            }
            else if (IS_EQUAL(cmd, search_ignore_case_cmd)) {
                CHECK_FD(fd);
                find(fd, arg, FT_CASE_IGNORE, memory);
            }
            else if (IS_EQUAL(cmd, position_cmd)) {
                CHECK_FD(fd);
                position = get_position(fd, arg);
                if (position < 0) {
                    continue;
                }
            }
            else if (IS_EQUAL(cmd, delete_cmd)) {
                CHECK_FD(fd);
                if (position < 0) {
                    printf("Position not set!\n");
                    continue;
                }
                count = to_int(arg, "Bad chars number!");
                if (count < 0) {
                    continue;
                }
                int64_t file_size = get_file_size(fd);
                if (file_size == 0) {
                    continue;
                }
                count = (count + position > file_size) ? file_size - position : count;
                remove_chars(fd, position, count);
            }
            else {
                printf("Unknown command! Try h for help.\n");
            }
        }
    }
}

#undef CHECK_FD
#undef IS_EQUAL

int main(int argc, char** argv) {
    int opt = 0;
    char* file_name = NULL;
    size_t memory = to_memory_value(NULL);
    int64_t position = -1;
    int64_t count = -1;
    open_mode mode = OM_EXIST;

    atexit(close_file_at_exit);

    if (argc == 1) {
        interactive_mode();
    }

    while ((opt = getopt(argc, argv, "m:f:s:S:p:a:d:h")) != -1) {
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
                mode = open_file(&fd, file_name);
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
            case 'p':
                check_fd(fd);
                position = get_position(fd, optarg);
                if (position < 0) {
                    exit(EXIT_FAILURE);
                }
                break;
            case 'a':
                check_fd(fd);
                if (position < 0) {
                    printf("Position not set!\n");
                    exit(EXIT_FAILURE);
                }
                add(fd, mode, position, optarg);
                break;
            case 'd':
                check_fd(fd);
                if (position < 0) {
                    printf("Position not set!\n");
                    exit(EXIT_FAILURE);
                }
                count = to_int(optarg, "Bad chars number!");
                if (count < 0) {
                    exit(EXIT_FAILURE);
                }
                int64_t file_size = (int64_t)get_file_size(fd);
                if (file_size == 0) {
                    exit(EXIT_SUCCESS);
                }
                count = (position + count > file_size) ? file_size - position : count;
                remove_chars(fd, position, count);
                count = -1;
                break;
            case 'h':
                printf(help_message);
                exit(EXIT_SUCCESS);
                break;
        }
    }
}
