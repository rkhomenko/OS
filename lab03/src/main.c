#include <matrix.h>

int main(int argc, char* argv[]) {
    mtx_t* mtx = NULL;
    mtx_err_t err = mtx_create(&mtx, 2, 3);
    mtx_read(mtx);
    mtx_print(mtx);
}
