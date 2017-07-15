#include <error.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void log_error_errno(const char* what,
                     const char* file,
                     int line,
                     const char* func) {
    const char* err_str = "Error \"%s\" in %s (%s:%d): %s\n";
    const char* errno_not_set = "Something goes wrong, but errno doesn't set!";

    if (errno == 0) {
        fprintf(stderr,
                err_str,
                what,
                func,
                file,
                line,
                errno_not_set);
        exit(ERRNO_NOT_SET);
    }

    fprintf(stderr,
            err_str,
            what,
            func,
            file,
            line,
            strerror(errno));
    exit(errno);
}

void log_error_msg(const char* file,
                   int line,
                   const char* func,
                   const char* msg,
                   int exit_code) {
    const char* err_str = "Error \"%s\" in %s (%s:%d)!\n";
    fprintf(stderr,
            err_str,
            msg,
            func,
            file,
            line);
    exit(exit_code);
}