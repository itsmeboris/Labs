#include "lab4_util.h"

#define SYS_WRITE 1
#define STDOUT 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_READ 0
#define SYS_EXIT 60
#define RWEPERMISSIONS 0777
#define SYS_GETDENTS 78

extern int system_call();

void error();
void print(char* output);

typedef struct  linux_dirent{
    long           d_ino;
    long          d_off;
    unsigned short d_reclen;
    char           d_name[];
} linux_dirent;

char* subDir(char* current,  char* res, char* dir){
    int i;
    for(i = 0 ; i < simple_strlen(current); i++)
        res[i] = current[i];
    res[i++]='/';
    for(int j = 0 ; j < simple_strlen(dir) ; j++, i++)
        res[i] = dir[j];
    res[i] ='\0';
    return res;
}

void getDir(char* path, char* nameToFind){
    int fd;
    int nread;
    char buf[1024];
    struct linux_dirent* directory;
    int bpos;
    char d_type;
    
    fd = system_call(SYS_OPEN,path, SYS_READ, RWEPERMISSIONS);
    if (fd == -1)
        error();
    for( ; ; ){
        nread = system_call(SYS_GETDENTS, fd, buf, 1024); 
        if (nread < 0)
            error();
        else if (nread == 0)
            break;
        for (bpos = 0; bpos < nread;) {
            directory = (struct linux_dirent*) (buf + bpos);
            d_type = *(buf + bpos + directory->d_reclen - 1);
            if ((simple_strcmp(directory->d_name ,".") != 0) && (simple_strcmp(directory->d_name,"..") != 0)){
                char newPath[simple_strlen(path) + simple_strlen(directory->d_name) + 2];
                subDir(path, newPath, directory->d_name);
                if(simple_strcmp("",nameToFind ) == 0)
                    print(newPath);
                else if (simple_strcmp(directory->d_name,nameToFind ) == 0)
                    print(newPath);
                if(d_type == 4)
                    getDir(newPath, nameToFind);
            }
            bpos += directory->d_reclen;
        }
    }
    system_call(SYS_CLOSE, fd);
}

int main (int argc , char* argv[], char* envp[])
{
    if (argc ==1){
        print(".");
        getDir(".", "");
    }
    return 0;
}


void error(){
    system_call(SYS_EXIT,0x55,0,0);
}

void print(char* output){
    system_call(SYS_WRITE,STDOUT,output,simple_strlen(output));
    system_call(SYS_WRITE,STDOUT, "\n", 1);
}