#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <text_editor.h>

int main(int argc, char** argv) {
    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        printf("Smt goes wrong!\n");
        exit(EXIT_FAILURE);
    }
    find(fd, "lol", FT_CASE_SENS, 4096);
    return 0;
}
