#include "lab4_util.h"

#define STDOUT 1
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 8
#define SYS_EXIT 60
#define RWEPERMISSIONS 0777
#define SHIRA 4117

extern int system_call();
void bye(){ 
    system_call(SYS_EXIT,0x55,0,0);
}

int main(int argc, char* argv[], char* envp[]){
    int fd = 0;
    char* name;
    fd = system_call(SYS_OPEN,argv[1],SYS_WRITE,RWEPERMISSIONS );
    if(fd < 0)
        bye();
    if(system_call(SYS_LSEEK,fd,SHIRA,0) < 0)
        bye();
    if(system_call(SYS_WRITE,fd,"      ",simple_strlen("      ")) < 0)  
        bye();
    if( system_call(SYS_LSEEK,fd,SHIRA,0) < 0)  
        bye();
    if(argc == 3){
        name = argv[2];
        if(system_call(SYS_WRITE,fd,name,simple_strlen(name)) < 0)
            bye();
        if(system_call(SYS_WRITE,fd,".",simple_strlen(".")) < 0)
            bye();
    }
    if(system_call(SYS_CLOSE,fd) < 0)
        bye();
    return 0;
}