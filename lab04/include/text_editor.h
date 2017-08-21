#ifndef OS_LABS_TEXT_EDITOR_H_
#define OS_LABS_TEXT_EDITOR_H_

enum find_type {
    FT_CASE_SENS,
    FT_CASE_IGNORE
};
typedef enum find_type find_type;

void find(int fd, const char* needle, find_type ft, size_t map_size);

#endif
