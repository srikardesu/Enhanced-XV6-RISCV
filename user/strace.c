#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//#include "string.h"

int main(int argc, char *argv[]) {
    int num = argc;
    if(num <= 2) {
        printf("Too Less Arguments!!\n");
        exit(1);
    }
    char ** s;
    s=malloc(100 * sizeof(char *));
    int sendmask = atoi(argv[1]);
    int start=0;
    for(int i=2;i<=argc;i++) {
        s[i-2]=argv[i]; 
        start++;  
    }
    //printf("%s\n",s);
    trace(sendmask);
    exec(argv[2],s);
    exit(1);
}
