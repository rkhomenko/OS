#include <error.h>
#include <file.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int file_open_r(const char* file_name) {
    int fd = open(file_name, O_RDONLY);

    if (fd < 0) {
        LOG_ERROR_ERRNO();
    }

    return fd;
}

void file_close(int fd) {
    if (close(fd) < 0) {
        LOG_ERROR_ERRNO();
    }
}

ssize_t file_read(int fd, void* buffer, size_t count) {
    return read(fd, buffer, count);
}