#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include <stdio.h>

static vec_t* vec_resize(vec_t* vec, size_t new_size) {
    vec->data = realloc(vec->data, vec->elem_size * new_size);

    if (vec->data == NULL) {
        return NULL;
    }

    vec->capacity = new_size;
    vec->size = (vec->size <= new_size) ? vec->size : new_size;

    return vec;
}

static void* vec_elem_default_ctor(void* elem) {
    return elem;
}

static void vec_elem_default_dctor(void* elem) {
    return;
}

vec_t* vec_create(size_t elem_size,
                  size_t size,
                  vec_elem_type type,
                  vec_elem_ctor_t ctor,
                  vec_elem_dctor_t dctor) {
    vec_t* vec = (vec_t*)malloc(sizeof(vec_t));

    if (vec == NULL) {
        return NULL;
    }

    vec->elem_size = elem_size;
    vec->size = vec->capacity = 0;
    vec->elem_type = type;
    vec->data = NULL;
    vec->ctor = ctor;
    vec->dctor = dctor;

    if (vec != vec_resize(vec, size)) {
        free(vec);
        return NULL;
    }

    return vec;
}

vec_t* vec_create_default(size_t elem_size,
                          vec_elem_type type,
                          size_t size) {
    return vec_create(elem_size,
                      size,
                      type,
                      vec_elem_default_ctor,
                      vec_elem_default_dctor);
}

void vec_destroy(vec_t* vec) {
    if (vec == NULL) {
        return;
    }

    switch (vec->elem_type) {
        case VEC_ELEM_TYPE_VAL:
            break;
        case VEC_ELEM_TYPE_PTR:
            for (size_t i = 0; i < vec->size; i++) {
                vec->dctor(VEC_I(vec, i, void**));
            }
            break;
    }


    free(vec->data);
    free(vec);
}

void vec_push_back(vec_t* vec, void* elem) {
    uint8_t* ptr = NULL;

    if (vec->size == vec->capacity) {
        vec_resize(vec, vec->capacity + 3);
    }

    switch (vec->elem_type) {
        case VEC_ELEM_TYPE_VAL:
            memcpy(VEC_I_PTR(vec, vec->size, uint8_t*),
                   (uint8_t*)vec->ctor(elem),
                   vec->elem_size);
            break;
        case VEC_ELEM_TYPE_PTR:
            ptr = vec->ctor(elem);
            memcpy(VEC_I_PTR(vec, vec->size, uint8_t*),
                   (uint8_t*)&ptr,
                   vec->elem_size);
            break;
    }
    vec->size++;
}