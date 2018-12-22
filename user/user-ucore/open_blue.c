#include <ulib.h>
#include <stdio.h>
#include <dir.h>
#include <error.h>
#define printf(...)                     fprintf(1, __VA_ARGS__)
int main(int argc, char **argv){
    int fd=open("/dev",O_RDONLY);
    printf("fd=%d\n\r",fd);
    fd=open("/bluetooth",O_RDONLY);
    printf("fd=%d\n\r",fd);
    if(fd<0){
        if(fd==-E_NOENT){
            printf("no such entry\n\r");
        }
        return 0;
    }
    char buf[100];
    while(1){
        int receive=write(fd,buf,1);
       // printf("%c",receive);
    } 
    return 0;
}