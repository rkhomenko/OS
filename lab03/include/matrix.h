#ifndef OS_LABS_MATRIX_H_
#define OS_LABS_MATRIX_H_

#include <stddef.h>

struct matrix {
    size_t n;
    size_t m;
    double* data;
};
typedef struct matrix mtx_t;

mtx_t* mtx_create(size_t n, size_t m);
void mtx_destroy(mtx_t* mtx);
void mtx_print(mtx_t* mxt);
void mtx_read(mtx_t* mtx);

#define MTX_N(mtx) ((mtx)->n)
#define MTX_M(mtx) ((mtx)->m)

#define MTX_I_J_PTR(mtx, i, j) \
    ((mtx)->data + (mtx)->n * (i) + (j))
#define MTX_I_J(mtx, i, j) (*MTX_I_J_PTR((mtx), (i), (j)))

#endif /* OS_LABS_MATRIX_H_ */
