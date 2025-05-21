#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int cnt = getprocs();
    printf("processes count: %d\n", cnt);
    exit(0);
}