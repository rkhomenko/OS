#include <matrix.h>

int main(int argc, char* argv[]) {
    mtx_t* mtx = mtx_create(2, 3);
    mtx_read(mtx);
    mtx_print(mtx);
}
