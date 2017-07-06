#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

int getline(char* buffer, size_t max) {
    int c = 0;
    size_t i = 0;

    while ((i < max) && ((c = getchar()) != '\n') && c != EOF) {
        buffer[i++] = (char)c;
    }

    buffer[i] = '\0';

    return i;
}

void* vec_str_ctor(void* elem) {
    size_t len = strlen((char*)elem);
    char* data = NULL;

    data = (char*)malloc(len + 1);
    strcpy(data, (char*)elem);

    return (void*)data;
}

void vec_str_dctor(void* elem) {
    free(elem);
}

int main(int argc, char* argv[]) {
    char buffer[1025];
    vec_t *vec = vec_create(sizeof(char*),
                            8,
                            VEC_ELEM_TYPE_PTR,
                            vec_str_ctor,
                            vec_str_dctor);

    while (!feof(stdin)) {
        size_t len = getline(buffer, 1024);
        vec_push_back(vec, (void*)buffer);
    }

    for (size_t i = 0; i < VEC_SIZE(vec); i++) {
        puts(VEC_I(vec, i, char**));
    }

    vec_destroy(vec);
    return 0;
}