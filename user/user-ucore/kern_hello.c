#include <ulib.h>
#include <stdio.h>
#include <dir.h>

#define printf(...)                     fprintf(1, __VA_ARGS__)
int main(int argc, char **argv){
    syscall(145);
    return 0;
}