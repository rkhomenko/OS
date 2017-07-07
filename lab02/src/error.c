#include <error.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* err_str = "Error (%s:%d <%s>): %s\n";

void log_error_errno(const char* file, int line, const char* func) {
    const char* errno_not_set = "Something goes wrong, but errno doesn't set!";

    if (errno == 0) {
        fprintf(stderr, err_str, file, line, func, errno_not_set);
        exit(ERRNO_NOT_SET);
    }

    fprintf(stderr,
            err_str,
            file,
            line,
            func,
            strerror(errno));
    exit(errno);
}

void log_error_msg(const char* file,
                   int line,
                   const char* func,
                   const char* msg,
                   int exit_code) {
    fprintf(stderr,
            err_str,
            file,
            line,
            func,
            msg);
    exit(exit_code);
}