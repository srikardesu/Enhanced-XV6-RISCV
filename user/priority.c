#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//#include "string.h"

int main(int argc, char *argv[]) {
    int pri = atoi(argv[1]);
    int pid = atoi(argv[2]);
    setpriority(pri,pid);
    exit(1);
}
