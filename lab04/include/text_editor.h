#ifndef OS_LABS_TEXT_EDITOR_H_
#define OS_LABS_TEXT_EDITOR_H_

#include <stddef.h>
#include <stdint.h>

enum find_type {
    FT_CASE_SENS,
    FT_CASE_IGNORE
};
typedef enum find_type find_type;

enum open_mode {
    OM_EXIST = -2,
    OM_CREAT = -3
};
typedef enum open_mode open_mode;

size_t get_file_size(int fd);
void find(int fd, const char* needle, find_type ft, size_t map_size);
void add(int fd, open_mode mode, int64_t pos, const char* what);
void add_from_stdin(int fd, open_mode mode, int64_t pos);

#endif
