#include <matrix.h>

#include <stdio.h>
#include <stdlib.h>

static const char* MTX_NO_ERR_STR = "MTX: No error";
static const char* MTX_ARGS_ERR_STR = "MTX: Bad arguments";
static const char* MTX_NULL_PTR_ERR_STR = "MTX: Null ptr";
static const char* MTX_DATA_ALLOC_ERR_STR = "MTX: Data memory allocation failed";
static const char* MTX_STRUCT_ALLOC_ERR_STR = "MTX: Struct memory allocation failed";
static const char* MTX_PRINT_DATA_ERR_STR = "MTX: Cannot print data";
static const char* MTX_READ_DATA_ERR_STR = "MTX: Cannot read data";

const char* mtx_strerror(mtx_err_t err) {
    const char* ret = NULL;
    switch(err) {
        case MTX_NO_ERR:
            ret = MTX_NO_ERR_STR;
            break;
        case MTX_ARGS_ERR:
            ret = MTX_ARGS_ERR_STR;
            break;
        case MTX_NULL_PTR_ERR:
            ret = MTX_NULL_PTR_ERR_STR;
            break;
        case MTX_STRUCT_ALLOC_ERR:
            ret = MTX_STRUCT_ALLOC_ERR_STR;
            break;
        case MTX_DATA_ALLOC_ERR:
            ret = MTX_DATA_ALLOC_ERR_STR;
            break;
        case MTX_PRINT_DATA_ERR:
            ret = MTX_PRINT_DATA_ERR_STR;
            break;
        case MTX_READ_DATA_ERR:
            ret = MTX_READ_DATA_ERR_STR;
            break;
    }
    return ret;
}

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
            if (printf("%5.2lf", MTX_I_J(mtx, i, j)) == 0) {
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

mtx_err_t mtx_extend(mtx_t** mtx, mtx_t* in, size_t filter_size) {
    if (mtx == NULL || in == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    if (filter_size == 0) {
        return MTX_ARGS_ERR;
    }

    size_t offset = filter_size / 2;
    mtx_err_t err = mtx_create(mtx,
                               MTX_N(in) + 2 * offset,
                               MTX_M(in) + 2 * offset);
    if (err != MTX_NO_ERR) {
        return err;
    }

    err = mtx_extend_exist(*mtx, in, filter_size);

    return MTX_NO_ERR;
}

mtx_err_t mtx_extend_exist(mtx_t* mtx, mtx_t* in, size_t filter_size) {
    if (mtx == NULL || in == NULL) {
        return MTX_NULL_PTR_ERR;
    }

    if (filter_size == 0) {
        return MTX_ARGS_ERR;
    }

    size_t offset = filter_size / 2;

    for (size_t i = offset; i < MTX_N(mtx) - offset; i++) {
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(in, i - offset, j - offset);
        }
    }

    // Matrix extension
    // top rows, left and right top squares
    for (size_t i = 0; i < offset; i++) {
        // top rows
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, offset, j);
        }
        // left top square
        for (size_t j = 0; j < offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, offset, offset);
        }
        // right top square
        for (size_t j = MTX_M(mtx) - offset; j < MTX_M(mtx); j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, offset, MTX_M(mtx) - offset - 1);
        }
    }

    // bottom rows, left and right bottom squares
    for (size_t i = MTX_N(mtx) - offset; i < MTX_N(mtx); i++) {
        // bottom rows
        for (size_t j = offset; j < MTX_M(mtx) - offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, MTX_N(mtx) - offset - 1, j);
        }
        // left bottom square
        for (size_t j = 0; j < offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, MTX_N(mtx) - offset - 1, offset);
        }
        // right bottom square
        for (size_t j = MTX_M(mtx) - offset; j < MTX_M(mtx); j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx,
                                         MTX_N(mtx) - offset - 1,
                                         MTX_M(mtx) - offset - 1);
        }
    }

    // left, right rows
    for (size_t i = offset; i < MTX_N(mtx) - offset; i++) {
        // left rows
        for (size_t j = 0; j < offset; j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, i, offset);
        }
        // right rows
        for (size_t j = MTX_M(mtx) - offset; j < MTX_M(mtx); j++) {
            MTX_I_J(mtx, i, j) = MTX_I_J(mtx, i, MTX_M(mtx) - offset - 1);
        }
    }

    return MTX_NO_ERR;
}
