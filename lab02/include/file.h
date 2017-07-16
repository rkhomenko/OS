#ifndef OS_LABS_FILE_H_
#define OS_LABS_FILE_H_

#include <unistd.h>

int file_open_r(const char* file_name);
void file_close(int fd);

ssize_t file_read(int fd, void* buffer, size_t count);

#endif /* OS_LABS_FILE_H_ */
