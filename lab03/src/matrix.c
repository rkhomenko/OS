#include <matrix.h>

#include <stdio.h>
#include <stdlib.h>

mtx_err_t mtx_create(mtx_t** mtx, size_t n, size_t m) {
    if (n == 0 || m == 0) {
        return MTX_ARGS_ERR;
    }

    if (mtx == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    *mtx =  (mtx_t*)malloc(sizeof(mtx_t));
    if (mtx == NULL) {
        return MTX_STRUCT_ALLOC_ERR;
    }

    MTX_DATA(*mtx) = (double*)malloc(sizeof(double) * m * n);
    if (MTX_DATA(*mtx) == NULL) {
        free(*mtx);
        return MTX_DATA_ALLOC_ERR;
    }

    MTX_N(*mtx) = n;
    MTX_M(*mtx) = m;

    return MTX_NO_ERR;
}

mtx_err_t mtx_destroy(mtx_t* mtx) {
    if (mtx == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    if (MTX_DATA(mtx) != NULL) {
        free(MTX_DATA(mtx));
    }

    free(mtx);

    return MTX_NO_ERR;
}

mtx_err_t mtx_print(mtx_t* mtx) {
    if (mtx == NULL || MTX_DATA(mtx) == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    for (size_t i = 0; i < MTX_N(mtx); i++) {
        for (size_t j = 0; j < MTX_M(mtx); j++) {
            if (printf("%lf", MTX_I_J(mtx, i, j)) == 0) {
                return MTX_PRINT_DATA_ERR;
            }
            if (j == MTX_M(mtx) - 1) {
                putchar('\n');
            }
            else {
                putchar(' ');
            }
        }
    }

    return MTX_NO_ERR;
}

mtx_err_t mtx_read(mtx_t* mtx) {
    if (mtx == NULL || MTX_DATA(mtx) == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    for (size_t i = 0; i < MTX_N(mtx); i++) {
        for (size_t j = 0; j < MTX_M(mtx); j++) {
            if (scanf("%lf", MTX_I_J_PTR(mtx, i, j)) != 1) {
                return MTX_READ_DATA_ERR;
            }
        }
    }

    return MTX_NO_ERR;
}

mtx_err_t mtx_read_and_extend(mtx_t* mtx, size_t filter_size) {
    if (mtx == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    size_t max = (MTX_N(mtx) < MTX_M(mtx)) ? MTX_M(mtx) : MTX_N(mtx);

    if (filter_size == 0 || filter_size >= max || filter_size % 2 != 1) {
        return MTX_ARGS_ERR;
    }

    size_t offset = filter_size / 2;

    for (size_t i = offset; i < MTX_N(mtx) - offset; i++) {
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            if (scanf("%lf", MTX_I_J_PTR(mtx, i, j)) != 1) {
                return MTX_READ_DATA_ERR;
            }
        }
    }

    /* top rows, left and right top squares */
    for (size_t i = 0; i < offset; i++) {
        /* top rows */
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, offset, j);
        }
        /* left top square */
        for (size_t j = 0; j < offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, offset, offset);
        }
        /* right top square */
        for (size_t j = MTX_M(mtx) - offset; j < MTX_M(mtx); j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, offset, MTX_M(mtx) - offset - 1);
        }
    }

    /* bottom rows, left and right bottom squares */
    for (size_t i = MTX_N(mtx) - offset; i < MTX_N(mtx); i++) {
        /* bottom rows */
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, MTX_N(mtx) - offset - 1, j);
        }
        /* left bottom square */
        for (size_t j = 0; j < offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, MTX_N(mtx) - offset - 1, offset);
        }
        /* right bottom square */
        for (size_t j = MTX_M(mtx) - offset; j < MTX_M(mtx); j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx,
                                         MTX_N(mtx) - offset - 1,
                                         MTX_M(mtx) - offset - 1);
        }
    }

    /* left, right rows */
    for (size_t i = offset; i < MTX_N(mtx) - offset; i++) {
        /* left rows */
        for (size_t j = 0; j < offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, i, offset);
        }
        /* right rows */
        for (size_t j = MTX_M(mtx) - offset; j < MTX_M(mtx); j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, i, MTX_M(mtx) - offset - 1);
        }
    }

    return MTX_NO_ERR;
}
