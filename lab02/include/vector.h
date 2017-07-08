#ifndef OS_LABS_VECTOR_H_
#define OS_LABS_VECTOR_H_

#include <stddef.h>
#include <stdint.h>

typedef enum vec_elem_type {
    VEC_ELEM_TYPE_VAL,
    VEC_ELEM_TYPE_PTR
} vec_elem_type;

typedef void* (*vec_elem_ctor_t)(void* elem_ptr);
typedef void (*vec_elem_dctor_t)(void* elem_ptr);

typedef struct vec {
    size_t elem_size;
    size_t size;
    size_t capacity;
    vec_elem_type elem_type;
    void* data;
    vec_elem_ctor_t ctor;
    vec_elem_dctor_t dctor;
} vec_t;

vec_t* vec_create(size_t elem_size,
                  size_t size,
                  vec_elem_type type,
                  vec_elem_ctor_t ctor,
                  vec_elem_dctor_t dctor);
vec_t* vec_create_default(size_t elem_size,
                          vec_elem_type type,
                          size_t size);
void vec_destroy(vec_t* vec);

void vec_push_back(vec_t* vec, void* elem);

void* vec_move_data(vec_t* vec);

#define VEC_SIZE(vec) \
    ((vec)->size)
#define VEC_DATA(vec) \
    ((vec)->data)

#define VEC_I_PTR(vec, i, type) \
    ((type)(((uint8_t*)(vec)->data) + (i) * (vec)->elem_size))

#define VEC_I(vec, i, type) \
    (*((type)(((uint8_t*)(vec)->data) + (i) * (vec)->elem_size)))

#endif /* OS_LABS_VECTOR_H_ */