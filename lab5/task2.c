#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "line_parser.h"

int main() {
    int fd[2];
    int status[2];
    pipe(fd); // fd[0] = read' fd[1] = write
    pid_t p1;
    pid_t p2;
    if((p1 = fork()) < 0){
        perror("failed first fork\n");
        _exit(-1);
    }
    else if(p1 == 0) { //this is a child process
        close(1);
        int fsout = dup(fd[1]);
        close(fd[1]);
        cmd_line* cmd = parse_cmd_lines("ls -l");
        execvp(cmd->arguments[0], cmd->arguments);
    }
    else{
        close(fd[1]);
        if((p2 = fork()) < 0){
            perror("failed first fork\n");
            _exit(-1);
        }
        else if(p2 == 0) { //this is a child process
            close(0);
            int fdin = dup(fd[0]);
            close(fd[0]);
            cmd_line* cmd = parse_cmd_lines("tail -n 2");
            execvp(cmd->arguments[0], cmd->arguments);
        }
        else{
            close(fd[0]);
            while(wait(&status[0]) != p1);
            while(wait(&status[1]) != p2);
        }
    }
}