#include <vector.h>

#include <stdlib.h>

static size_t DEFAULT_SIZE = 10;

vec_err_t vec_create(vec_t* v, size_t size) {
    vec_data(v) = NULL;
    vec_size(v) = vec_capacity(v) = 0;

    vec_err_t err = vec_resize(v, size);
    if (err != VE_NOERR) {
        return err;
    }

    return VE_NOERR;
}

vec_err_t vec_resize(vec_t* v, size_t new_size) {
    size_t size = (new_size == 0) ? DEFAULT_SIZE : new_size;

    data_t* tmp = (data_t*)realloc(vec_data(v), sizeof(data_t) * size);
    if (tmp == NULL) {
        return VE_ERROR;
    }

    vec_data(v) = tmp;
    vec_capacity(v) = size;
    vec_size(v) = (vec_size(v) > size) ? size : vec_size(v);
    vec_size(v) = (new_size == 0) ? DEFAULT_SIZE : vec_size(v);

    return VE_NOERR;
}

vec_err_t vec_push(vec_t* v, data_t value) {
    vec_err_t err = VE_NOERR;

    if (vec_size(v) == vec_capacity(v)) {
        err = vec_resize(v, vec_capacity(v) * 2);
        if (err != VE_NOERR) {
            return err;
        }
    }

    vec_index(v, vec_size(v)) = value;
    vec_size(v)++;

    return err;
}

void vec_destroy(vec_t* v) {
    if (vec_data(v) != NULL) {
        free(vec_data(v));
    }

    vec_capacity(v) = vec_size(v) = 0;
}
