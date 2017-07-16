#include <error.h>
#include <file.h>
#include <vector.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void* vec_str_ctor(void* elem) {
    return elem;
}

static void vec_str_dctor(void* elem) {
    free(elem);
}

enum error_codes {
    BAD_ARGUMENTS = ERRNO_NOT_SET + 1,
    CNT_CRT_CHL_PROC
};

static vec_t* load_file(int fd) {
    char NULL_CHAR = '\0';
    char c = '\0';
    vec_t* v = vec_create_default(sizeof(char), VEC_ELEM_TYPE_VAL, 128);
    vec_t* ret = vec_create(sizeof(char*),
                            128,
                            VEC_ELEM_TYPE_PTR,
                            vec_str_ctor,
                            vec_str_dctor);

    while (file_read(fd, &c, 1) == 1) {
        if (c == '\n') {
            vec_push_back(v, &NULL_CHAR);
            vec_push_back(ret, vec_move_data(v));
        } else {
            vec_push_back(v, &c);
        }
    }

    if (VEC_SIZE(v) != 0) {
        vec_push_back(v, &NULL_CHAR);
        vec_push_back(ret, vec_move_data(v));
    }

    vec_destroy(v);

    return ret;
}

static int compare_elems(const char* lhs, const char* rhs) {
    return strcmp(lhs, rhs) > 0;
}

static void sort_vec(vec_t* vec) {
    for (int i = 1; i < (int)VEC_SIZE(vec); i++) {
        int j = i;
        while (j > 0 &&
               compare_elems(VEC_I(vec, j - 1, char**),
                             VEC_I(vec, j, char**))) {
            char* tmp = VEC_I(vec, j - 1, char**);
            VEC_I(vec, j - 1, char**) = VEC_I(vec, j, char**);
            VEC_I(vec, j, char**) = tmp;
            j--;
        }
    }
}

static void write_to_pipe(int fd, vec_t* vec) {
    write(fd, &VEC_SIZE(vec), sizeof(size_t));
    for (size_t i = 0; i < VEC_SIZE(vec); i++) {
        size_t len = strlen(VEC_I(vec, i, char**)) + 1;
        write(fd, &len, sizeof(size_t));
        write(fd, VEC_I(vec, i, char**), len);
    }
}

static int read_from_pipe(int fd, int res) {
    size_t count = 0;
    size_t len = 0;
    size_t str_len = 0;

    if (res == -1) {
        len = read(fd, &count, sizeof(size_t));
        if (len != sizeof(size_t)) {
            return 0;
        }
    }

    len = read(fd, &str_len, sizeof(size_t));

    if (len == 0) {
        return 0;
    }

    vec_t* v = vec_create_default(sizeof(char), VEC_ELEM_TYPE_VAL, str_len);
    len = read(fd, VEC_DATA(v), str_len);

    puts(VEC_DATA(v));

    vec_destroy(v);

    return (res == -1) ? (int)(count - 1) : res - 1;
}

static void in_child_proc(int fd, int out_pipe_fd) {
    vec_t* v = load_file(fd);
    file_close(fd);

    sort_vec(v);
    write_to_pipe(out_pipe_fd, v);
    vec_destroy(v);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        LOG_ERROR_MSG("Incorrect arguments count! Need two!",
                      BAD_ARGUMENTS);
    }

    int fd1 = file_open_r(argv[1]);
    int fd2 = file_open_r(argv[2]);

    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) < 0) {
        LOG_ERROR_ERRNO("Cannot create pipe1");
    }

    if (pipe(pipe2) < 0) {
        LOG_ERROR_ERRNO("Cannot create pipe2");
    }

    pid_t id = fork();

    if (id < 0) {
        LOG_ERROR_ERRNO("Cannot fork for child1");
    }

    if (id != 0) { /* Parent process continue */
        close(pipe1[1]);
        close(pipe2[1]);

        wait(NULL);

        int res1 = read_from_pipe(pipe1[0], -1);
        int res2 = read_from_pipe(pipe2[0], -1);
        while (res1 || res2) {
            if (res1) {
                res1 = read_from_pipe(pipe1[0], res1);
            }
            if (res2) {
                res2 = read_from_pipe(pipe2[0], res2);
            }
        }
        close(pipe1[0]);
        close(pipe2[0]);
    }
    else { /* Child1 proc started */
        id = fork();
        if (id < 0) {
            LOG_ERROR_ERRNO("Cannot for child2");
        }

        if (id != 0) { /* Child1 proc continue */
            close(pipe1[0]); /* close unused read pipe */
            in_child_proc(fd1, pipe1[1]);
            close(pipe1[1]);
        }
        else { /* Child2 proc started */
            close(pipe2[0]); /* close unused read pipe */
            in_child_proc(fd2, pipe2[1]);
            close(pipe2[1]);
        }
    }

    return 0;
}
