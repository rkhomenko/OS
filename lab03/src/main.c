#include <matrix.h>

int main(int argc, char* argv[]) {
    mtx_t* mtx = NULL;
    mtx_err_t err = mtx_create(&mtx, 5, 6);
    mtx_read_and_extend(mtx, 5);
    mtx_print(mtx);
    mtx_destroy(mtx);
}
