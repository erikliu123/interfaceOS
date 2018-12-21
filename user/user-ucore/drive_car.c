#include <ulib.h>
#include <stdio.h>
#include <dir.h>
#include <error.h>
#define printf(...)                     fprintf(1, __VA_ARGS__)
int main(int argc, char **argv){
    int fd=open("bluetooth",O_RDWR);//不加/
    int fd_car=open("car",O_RDWR);
    int speed=1,dir=2;
    char buf[100];
    int *bufint=(int *)buf;
    if(fd<0){
        if(fd==-E_NOENT || fd_car==-E_NOENT){
            printf("module should be installed\n\r");
        }
        return 0;
    }
    
    while(1){
        read(fd,buf,4);//如果返回的length > 要读取的长度，引起panic
        char s=buf[0];
        //printf("%c",s);
        switch(s){  
            case 'w':

            break;
            case 'a':

            break;
            case 's':

            break;
            case 'd':

            break;

            default:  
            close(fd);
            close(fd_car);
            return 0;
        }
        bufint[0]=speed;
        bufint[1]=dir;
        write(fd_car,buf,8);
       // printf("%c",receive);
    } 
    return 0;
}