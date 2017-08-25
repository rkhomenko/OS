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
    for (size_t i = 0; i < size; i++) {
        zblocks[i] = 0;
    }


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
                              char* buffer,
                              char* last,
                              size_t* zblocks,
                              size_t* chars) {
    const size_t NEEDLE_SIZE = strlen(needle);
    size_t copy_size = 0;
    size_t last_size = strlen(last);
    size_t index = NEEDLE_SIZE;

    for (size_t i = 0; i < last_size; i++) {
        buffer[index] = last[i];
        index++;
    }

    for (size_t i = 0; i < text_size; i++) {
        switch (ft) {
            case FT_CASE_SENS:
                buffer[index] = text[i];
                break;
            case FT_CASE_IGNORE:
                buffer[index] = tolower(text[i]);
                break;
        }
        index++;
    }

    size_t buffer_size = index;
    build_zblocks(buffer, zblocks, buffer_size);

    for (size_t i = NEEDLE_SIZE; i < buffer_size; i++) {
        (*chars)++;
        if (zblocks[i] == NEEDLE_SIZE) {
            printf("%lu\n", *chars);
        }
    }

    last_size = 0;
    for (size_t i = buffer_size - NEEDLE_SIZE + 1; i < buffer_size; i++) {
        if (zblocks[i] + i == buffer_size) {
            copy_size = zblocks[i];
            break;
        }
    }

    *chars -= copy_size;

    size_t last_index = 0;
    for (size_t i = buffer_size - copy_size; i < buffer_size; i++) {
        last[last_index] = buffer[i];
        last_index++;
    }

    last[copy_size] = '\0';
}

void find(int fd, const char* needle, find_type ft, size_t map_size) {
    const size_t FILE_SIZE = get_file_size(fd);
    const size_t COUNT = FILE_SIZE / map_size +
                         ((FILE_SIZE % map_size == 0) ? 0 : 1);
    const size_t NEEDLE_SIZE = strlen(needle);
    const size_t BUFFER_SIZE = 2 * NEEDLE_SIZE + map_size;
    size_t readed = 0;
    char* mapped = NULL;
    char* buffer = NULL;
    char* last = NULL;
    size_t* zblocks = NULL;
    size_t chars = 0;

    last = (char*)malloc(NEEDLE_SIZE);
    buffer = (char*)malloc(BUFFER_SIZE);
    zblocks = (size_t*)malloc(BUFFER_SIZE * sizeof(size_t));

    *last = '\0';

    for (size_t i = 0; i < NEEDLE_SIZE; i++) {
        switch (ft) {
            case FT_CASE_SENS:
                buffer[i] = needle[i];
                break;
            case FT_CASE_IGNORE:
                buffer[i] = tolower(needle[i]);
                break;
        }
    }

    for (size_t i = 0; i < COUNT; i++) {
        size_t current_map_size =
            (FILE_SIZE - readed < map_size) ? FILE_SIZE - readed : map_size;
        mapped = mmap(mapped, current_map_size, PROT_READ, MAP_PRIVATE, fd, i * map_size);
        find_with_zblocks(mapped, current_map_size, ft, needle, buffer, last, zblocks, &chars);
        munmap(mapped, current_map_size);
        readed += current_map_size;
    }

    free(last);
    free(buffer);
    free(zblocks);
}
