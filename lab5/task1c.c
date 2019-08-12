#include <unistd.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#include "linux/limits.h"

#include "line_parser.h"

#define FREE(X) if(X) free((void*)X)

void execute(cmd_line *line);

int main(){
    char* buff;
    int max_chars = 2048;
    cmd_line* cmd;
    while(1){
        buff = malloc(PATH_MAX);
        if (getcwd(buff, PATH_MAX) != NULL)
            fprintf(stdout, "%s:", buff);
        else {
            FREE(buff);
            perror("getcwd failed");
            _exit(-1);
        }
        FREE(buff);
        buff = malloc(max_chars);
        fgets(buff, max_chars, stdin);
        if(strcmp(buff, "quit\n") == 0) {
            FREE(buff);
            _exit(0);
        }
        else {
            cmd = parse_cmd_lines(buff);
            FREE(buff);
            if(cmd)
                execute(cmd);
            free_cmd_lines(cmd);
        }
    }
}

void execute(cmd_line *line){
    pid_t pid;
    int status;
    if((pid = fork()) < 0){
        perror("*** ERROR: forking child process failed\n");
        free_cmd_lines(line);
        _exit(1);
    }
    else if(pid == 0) { //this is a child process
        if(line->output_redirect){
            fclose(stdout);
            stdout = fopen(line->output_redirect,"w+");
            if(!stdout) {
                perror("open failed: ");
                free_cmd_lines(line);
                _exit(-1);
            }
        }
        if(line->input_redirect){
            fclose(stdin);
            stdin = fopen(line->input_redirect, "r");
            if(!stdin) {
                perror("open failed: ");
                if(line->output_redirect)
                    fclose(stdout);
                free_cmd_lines(line);
                _exit(-1);
            }
        }
        if(execvp(line->arguments[0],line->arguments) < 0) {
            perror("*** ERROR: exec failed\n");
            _exit(-1);
        }

        if(line->input_redirect)
            fclose(stdin);
        if(line->output_redirect)
            fclose(stdout);
    }
    else{
        if(line->blocking != 0){
            while(waitpid(-1, &status, 0) != pid);
        }
        if(line->next != NULL)
            execute(line->next);
    }
}

