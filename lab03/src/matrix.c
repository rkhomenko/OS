#include <matrix.h>

#include <stdio.h>
#include <stdlib.h>

mtx_t* mtx_create(size_t n, size_t m) {
    if (n == 0 || m == 0) {
        return 0;
    }

    mtx_t* mtx =  (mtx_t*)malloc(sizeof(mtx_t));
    if (mtx == NULL) {
        return NULL;
    }

    mtx->data = (double*)malloc(sizeof(double) * m * n);
    if (mtx->data == NULL) {
        free(mtx);
        return NULL;
    }

    mtx->n = n;
    mtx->m = m;

    return mtx;
}

void mtx_destroy(mtx_t* mtx) {
    if (mtx == NULL) {
        return;
    }

    if (mtx->data != NULL) {
        free(mtx->data);
    }

    free(mtx);
}

void mtx_print(mtx_t* mtx) {
    if (mtx == NULL) {
        return;
    }

    for (size_t i = 0; i < MTX_N(mtx); i++) {
        for (size_t j = 0; j < MTX_M(mtx); j++) {
            printf("%.2lf", MTX_I_J(mtx, i, j));
            if (j == MTX_M(mtx) - 1) {
                putchar('\n');
            }
            else {
                putchar(' ');
            }
        }
    }
}

void mtx_read(mtx_t* mtx) {
    if (mtx == NULL || mtx->data == NULL) {
        return;
    }

    for (size_t i = 0; i < MTX_N(mtx); i++) {
        for (size_t j = 0; j < MTX_M(mtx); j++) {
            if (scanf("%lf", MTX_I_J_PTR(mtx, i, j)) != 1) {
                printf("Cannot read data!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
