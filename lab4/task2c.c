#include "lab4_util.h"

#define SYS_READ 0
#define SYS_WRITE 1
#define STDOUT 1
#define STDERR 2
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_EXIT 60
#define WAIT 61
#define RWEPERMISSIONS 0777
#define SYS_GETDENTS 78

extern int system_call();

typedef struct  linux_dirent{
    long           d_ino;
    long          d_off;
    unsigned short d_reclen;
    char           d_name[];
} linux_dirent;

void bye();
void print(char * output);

char* subDire(char* current,  char* res, char* dir){
    int i;
    for(i = 0 ; i < simple_strlen(current); i++)
        res[i] = current[i];
    res[i]='/';
    i++;
    for(int j = 0 ; j < simple_strlen(dir) ; j++, i++)
        res[i] = dir[j];
    res[i] ='\0';
    return res;
}

char* subDir(char* old, char* toAdd, char* new, int command){
    int i;
    for(i =0 ; i<simple_strlen(old); i++)
        new[i] = old[i];
    if (!command)
        new[i]='/';
    else
        new[i]=' ';
    i++;
    for(int j=0 ; j<simple_strlen(toAdd) ; j++, i++)
        new[i] = toAdd[j];
    new[i] ='\0';
    return new;
}


int creatCommand(char* path, char* nameToFind, char* command){
    int fd;
    int nread;
    char buf[1024];
    linux_dirent* directory;
    int bpos;
    char d_type;    
    int found = 0;
    
    fd = system_call(SYS_OPEN,path, SYS_READ, RWEPERMISSIONS);
    if (fd == -1)
        bye();
    for( ; ; ){
        nread = system_call(SYS_GETDENTS, fd, buf, 1024);
        if (nread == -1)
            bye();
        if (nread == 0)
            break;
        for (bpos = 0; bpos < nread;) {
            directory = (linux_dirent*) (buf + bpos);
            d_type = *(buf + bpos + directory->d_reclen - 1);
            if ((simple_strcmp(directory->d_name ,".")) && (simple_strcmp(directory->d_name,".."))){
                if (!simple_strcmp(directory->d_name,nameToFind )){
                    char newPath[simple_strlen(path)+simple_strlen(directory->d_name)+2];
                    subDir(path, directory->d_name, newPath, 0);
                    char commandToSend[simple_strlen(command)+simple_strlen(nameToFind)+2];
                    subDir(command, newPath, commandToSend, 1);
                    simple_system(commandToSend);
                    system_call(WAIT,0);
                    found = 1;
                }
                if(d_type == 4){
                    char newPath[simple_strlen(path)+simple_strlen(directory->d_name)+2];
                    subDir(path, directory->d_name, newPath, 0);
                    found = creatCommand(newPath, nameToFind, command);
                }
            }
            bpos += directory->d_reclen;
        }
        
    }
    system_call(SYS_CLOSE, fd);
    return found;
}

void getDir(char* path, char* nameToFind){
    int fd;
    int nread;
    char buf[1024];
    linux_dirent* directory;
    int bpos;
    char d_type;
    
    fd = system_call(SYS_OPEN,path, SYS_READ, RWEPERMISSIONS);
    if (fd == -1)
        bye();
    for( ; ; ){
        nread = system_call(SYS_GETDENTS, fd, buf, 1024); 
        if (nread == -1)
            bye();
        else if (nread == 0)
            break;
        for (bpos = 0; bpos < nread;) {
            directory = (linux_dirent*) (buf + bpos);
            d_type = *(buf + bpos + directory->d_reclen - 1);
            if ((simple_strcmp(directory->d_name ,".")) && (simple_strcmp(directory->d_name,".."))){
                char newPath[simple_strlen(path) + simple_strlen(directory->d_name) + 2];
                subDire(path, newPath, directory->d_name);
                if(!simple_strcmp("",nameToFind ))
                    print(newPath);
                else if (!simple_strcmp(directory->d_name,nameToFind ))
                    print(newPath);
                if(d_type ==4){
                    getDir(newPath, nameToFind);}
            }
            bpos += directory->d_reclen;
        }
    }
    system_call(SYS_CLOSE, fd);
}

int main (int argc , char* argv[], char* envp[])
{
    if (argc ==4){
        if(!creatCommand(".", argv[2], argv[3])){
            system_call(SYS_WRITE,STDOUT,"The file ",simple_strlen("The file "));
            system_call(SYS_WRITE,STDOUT,argv[2],simple_strlen(argv[2]));
            system_call(SYS_WRITE,STDOUT," Does not exist.",simple_strlen(" Does not exist."));
            system_call(SYS_WRITE,STDOUT, "\n", 1);
            bye();
        }
    }
    else if(argc == 3)
        getDir(".", argv[2]);
    else{
        print(".");
        getDir(".", "");
    }
    return 0;
}

void bye(){
    system_call(SYS_EXIT,0x55,0,0);
}

void print(char * output){
    system_call(SYS_WRITE,STDOUT,output,simple_strlen(output));
    system_call(SYS_WRITE,STDOUT, "\n", 1);
}
