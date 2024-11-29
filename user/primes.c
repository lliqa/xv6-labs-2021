#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void solve(int f, int pr) {
    fprintf(1, "prime %d\n", pr);

    int p[2];
    pipe(p);

    int nxtpr = 0, x;
    while (read(f, &x, sizeof(x)) == sizeof(x)) {
        if (x % pr == 0) continue;
        if (!nxtpr) nxtpr = x;
        if (write(p[1], &x, sizeof(x)) != sizeof(x)) {
            fprintf(2, "write failed\n");
            exit(1);
        }
    }
    close(f);
    
    if (!nxtpr) exit(0);
    
    if (fork() != 0) {
        close(p[0]);
        close(p[1]);
        wait((int *)0);
    }else {
        close(p[1]);
        solve(p[0], nxtpr);
    }
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    for (int i = 2; i <= 35; ++i) {
        if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
            fprintf(2, "write failed\n");
            exit(1);
        }
    }

    if (fork() != 0) {
        close(p[0]);
        close(p[1]);
        wait((int *)0);
    }else {
        close(p[1]);
        solve(p[0], 2);
    }
    exit(0);
}
