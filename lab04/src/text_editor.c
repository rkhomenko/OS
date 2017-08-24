#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <text_editor.h>

static size_t get_file_size(int fd) {
    struct stat st;
    if (fstat(fd, &st) < 0) {
        printf("get_file_size: fstat error\n");
        exit(EXIT_FAILURE);
    }
    return st.st_size;
}

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static void build_zblocks(char* text, size_t* zblocks, size_t size) {
    for (size_t i = 1, left = 0, right = 0; i < size; ++i) {
        if (i <= right) {
            zblocks[i] = MIN(right - i + 1, zblocks[i - left]);
        }
        while (i + zblocks[i] < size && text[zblocks[i]] == text[i + zblocks[i]]) {
            ++zblocks[i];
        }
        if (i + zblocks[i] - 1 > right) {
            left = i;
            right = i + zblocks[i] - 1;
        }
    }
}

#undef MIN

static void find_with_zblocks(const char* text,
                              size_t text_size,
                              find_type ft,
                              const char* needle,
                              char** last,
                              size_t* lines,
                              size_t* chars) {
    const size_t NEEDLE_SIZE = strlen(needle);
    size_t last_size = 0;
    size_t index = 0;

    if (*last != 0) {
        last_size = strlen(*last);
    }

    size_t buffer_size = NEEDLE_SIZE + text_size + last_size;
    char* buffer = (char*)malloc(buffer_size);
    size_t* zblocks = (size_t*)malloc(buffer_size * sizeof(size_t));

    for (size_t i = 0; i < NEEDLE_SIZE; i++) {
        switch(ft) {
            case FT_CASE_SENS:
                buffer[index] = needle[i];
                break;
            case FT_CASE_IGNORE:
                buffer[index] = tolower(needle[i]);
                break;
        }
        index++;
    }

    if (*last != NULL) {
        for (size_t i = 0; i < last_size; i++) {
            buffer[index] = (*last)[i];
            index++;
        }
    }

    for (size_t i = 0; i < text_size; i++) {
        switch(ft) {
            case FT_CASE_SENS:
                buffer[index] = text[i];
                break;
            case FT_CASE_IGNORE:
                buffer[index] = tolower(text[i]);
                break;
        }
        index++;
    }

    build_zblocks(buffer, zblocks, buffer_size);

    for (size_t i = NEEDLE_SIZE; i < buffer_size; i++) {
        if (buffer[i] == '\n') {
            ++*lines;
            *chars = 0;
            continue;
        }
        else {
            (*chars)++;
        }
        if (zblocks[i] == NEEDLE_SIZE) {
            printf("Find in line %lu, column %lu!\n", *lines, *chars);
        }
    }

    free(buffer);
    free(zblocks);
}

void find(int fd, const char* needle, find_type ft, size_t map_size) {
    const size_t FILE_SIZE = get_file_size(fd);
    const size_t COUNT = FILE_SIZE / map_size +
                         ((FILE_SIZE % map_size == 0) ? 0 : 1);
    size_t readed = 0;
    char* mapped = NULL;
    char* last = NULL;
    size_t lines = 1;
    size_t chars = 0;

    for (size_t i = 0; i < COUNT; i++) {
        size_t current_map_size =
            (FILE_SIZE - readed < map_size) ? FILE_SIZE - readed : map_size;
        mapped = mmap(mapped, current_map_size, PROT_READ, MAP_PRIVATE, fd, i * map_size);
        find_with_zblocks(mapped, current_map_size, ft, needle, &last, &lines, &chars);
        munmap(mapped, current_map_size);
        readed += current_map_size;
    }
}
