#include <ulib.h>
#include <stdio.h>
#include <dir.h>
#include <error.h>
#define printf(...)                     fprintf(1, __VA_ARGS__)
int main(int argc, char **argv){
   
    int i=0;
    for(i=1;i<argc;++i){
        int ret=syscall(142,argv[i]);
        if(ret==0){
            printf("\"%s\"success!\n\r",argv[i]);
            continue;
        }
        if(ret==-E_NOMEM){
            printf("have not enough memory for create inode for \"%s\"\n\r",argv[i]);
        }
        else if(ret==-E_EXIST){
            printf("the device \"%s\" has already been mounted\n\r",argv[i]);
        }
        else  printf("the device \"%s\" has failed for other reasons\n\r",argv[i]);
        return 0;
        
    }
    return 0;
}