#ifndef OS_LABS_MATRIX_H_
#define OS_LABS_MATRIX_H_

#include <stddef.h>

struct matrix {
    size_t n;
    size_t m;
    double* data;
};
typedef struct matrix mtx_t;

#define MTX_DATA(mtx) ((mtx)->data)
#define MTX_N(mtx) ((mtx)->n)
#define MTX_M(mtx) ((mtx)->m)

#define MTX_I_J_PTR(mtx, i, j) \
    (MTX_DATA(mtx) + MTX_M(mtx) * (i) + (j))
#define MTX_I_J(mtx, i, j) (*MTX_I_J_PTR(mtx, i, j))

enum matrix_error {
    MTX_NO_ERR,
    MTX_ARGS_ERR,
    MTX_NULL_PTR_ERR,
    MTX_STRUCT_ALLOC_ERR,
    MTX_DATA_ALLOC_ERR,
    MTX_PRINT_DATA_ERR,
    MTX_READ_DATA_ERR
};
typedef enum matrix_error mtx_err_t;

mtx_err_t mtx_create(mtx_t** mtx, size_t n, size_t m);
mtx_err_t mtx_destroy(mtx_t* mtx);
mtx_err_t mtx_print(mtx_t* mxt);
mtx_err_t mtx_read(mtx_t* mtx);
mtx_err_t mtx_read_and_extend(mtx_t* mtx, size_t filter_size);

#endif /* OS_LABS_MATRIX_H_ */
