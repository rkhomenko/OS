#include <vector.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define VOID(func) (*(void **)(&(func)))

int main(int argc, char** argv) {
    vec_t vec;
    vec_create_t create = NULL;
    vec_push_t push = NULL;
    vec_destroy_t destroy = NULL;
    data_t value = 0;
    data_t result = 0;

    if (argc != 2) {
        printf("Bad arguments\n");
        exit(EXIT_FAILURE);
    }

    void* handle = dlopen(argv[1], RTLD_NOW);
    if (handle == NULL) {
        printf("Cannot load shared library: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    dlerror();

    VOID(create) = dlsym(handle, "vec_create");
    VOID(push) = dlsym(handle, "vec_push");
    VOID(destroy) = dlsym(handle, "vec_destroy");

    create(&vec, 3);

    while (scanf("%i", &value) == 1) {
        push(&vec, value);
    }

    for (int i = 0; i < (data_t)vec_size(&vec); i++) {
        result += vec_index(&vec, i);
    }
    printf("%i\n", result);

    destroy(&vec);

    dlclose(handle);

    return 0;
}
