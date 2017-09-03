#ifndef OS_LABS_VECTOR_H_
#define OS_LABS_VECTOR_H_

#include <stddef.h>
#include <stdint.h>

typedef int32_t data_t;

struct vector {
    data_t* data;
    size_t size;
    size_t capacity;
};
typedef struct vector vec_t;

#define vec_data(v) ((v)->data)
#define vec_size(v) ((v)->size)
#define vec_capacity(v) ((v)->capacity)
#define vec_empty(v) (vec_size(v) == 0)
#define vec_index(v, i) (vec_data(v)[(i)])

enum vector_error {
    VE_NOERR,
    VE_ERROR
};
typedef enum vector_error vec_err_t;

vec_err_t vec_create(vec_t* v, size_t size);
vec_err_t vec_resize(vec_t* v, size_t new_size);
vec_err_t vec_push(vec_t* v, data_t value);
void vec_destroy(vec_t* v);

typedef vec_err_t (*vec_create_t)(vec_t* v, size_t size);
typedef vec_err_t (*vec_resize_t)(vec_t* v, size_t new_size);
typedef vec_err_t (*vec_push_t)(vec_t* v, data_t value);
typedef void (*vec_destroy_t)(vec_t* v);

#endif /* OS_LABS_VECTOR_H_ */
