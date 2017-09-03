#include <vector.h>

#include <stdio.h>

int main(void) {
    vec_t vec;
    data_t value = 0;
    data_t result = 0;

    vec_create(&vec, 3);

    while (scanf("%i", &value) == 1) {
        vec_push(&vec, value);
    }

   for (int i = 0; i < (data_t)vec_size(&vec); i++) {
        result += vec_index(&vec, i);
    }
    printf("%i\n", result);

    vec_destroy(&vec);
    return 0;
}
