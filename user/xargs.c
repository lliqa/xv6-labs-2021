#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"


int getline(char *buf, int siz) {
    int cnt = 0;
    while (read(0, buf, sizeof(char)) == sizeof(char)) {
        ++cnt;
        if (cnt > siz) {
            fprintf(2, "too long");
            exit(1);
        }
        if(*buf == '\n') {
            *buf = 0;
            break;
        }
        ++buf;
    }
    return cnt;
}

void makeArg(char **ap, char *buf, int siz) {
    int cnt = 0;
    while (*buf) {
        while (*buf == ' ') ++buf;
        if (*buf == 0) break;

        if (cnt == siz) break;
        ap[cnt++] = buf;
        while (*buf && *buf != ' ') ++buf;
        if (*buf) *buf = 0, ++buf;
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "Too few arguments\n");
        exit(1);
    }

    char buf[512] = {0};

    int argcn = argc - 1;
    char *arg[MAXARG] = {0};
    for (int i = 1; i < argc; ++i) arg[i - 1] = argv[i];

    while (getline(buf, 512)) {
        fprintf(1, "%s\n", buf);
        makeArg(arg + argcn, buf, MAXARG - argcn);
        
        if (fork() != 0) wait((int *)0);
        else exec(arg[0], arg);
    }
    exit(0);
}
