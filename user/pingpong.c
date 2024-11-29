#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    char msg[] = "1";
    char buf[2];

    if (fork() == 0) {
        if (read(p[0], buf, sizeof(buf)) != 1) {
            fprintf(2, "pingpong: read failed\n");
            exit(1);
        }
        close(p[0]);
        fprintf(1, "%d: received ping\n", getpid());

        if (write(p[1], msg, 1) != 1) {
            fprintf(2, "pingpong: write failed\n");
            exit(1);
        }
        close(p[1]);
    }else {
        if (write(p[1], msg, 1) != 1) {
            fprintf(2, "pingpong: write failed\n");
            exit(1);
        }
        close(p[1]);

        if (read(p[0], buf, sizeof(buf)) != 1) {
            fprintf(2, "pingpong: read failed\n");
            exit(1);
        }
        fprintf(1, "%d: received pong\n", getpid());        
        close(p[0]);
    }
    exit(0);
}
