#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "line_parser.h"
#include "job_control.h"

#define MAX_USER_INPUT 2048

#define FREE(X) if(X) free((void*)X)


void setSignals();
void setSignalsDefault();
void printDir(char* buffer);
void debugger(char * msg);
void catch_signals(int signal);
int preProcess(cmd_line* cmdLines);
void execute(cmd_line* cmd_line1, job* currJob, int* pipeLeft, int* pipeRight, int first_child);
void setIgnoreSignals();
void setSignalsDefault();

char * termios = NULL;
job* jobs = NULL;
int debug = 0;
struct termios * shell_attr;
pid_t shell_pid;

int main(int argc, char **argv) {
    char buffer[MAX_USER_INPUT];
    char cwd[PATH_MAX];
    int keepGoing = 1;
    setIgnoreSignals();
    setSignals();
    setpgid(getpid(), getpid());
    shell_attr = (struct termios *) malloc(sizeof(struct termios));
    shell_pid = getpid();
    tcgetattr(STDIN_FILENO, shell_attr);
    tcsetpgrp(STDIN_FILENO, shell_pid); // grab controll of the shell
    if (argc > 1 && strcmp(argv[1], "-d") == 0) debug = 1;
    while (keepGoing) {
        printDir(cwd);
        if (fgets(buffer, MAX_USER_INPUT, stdin) != NULL) {
            cmd_line *cmdLines = parse_cmd_lines(buffer);
            if (cmdLines) {
                if (strcmp(cmdLines->arguments[0], "quit") != 0) {
                    if (preProcess(cmdLines) != 0) {
                        int fd[2];
                        job *j = add_job(&jobs, buffer);
                        j->status = RUNNING;
                        if (cmdLines->next) {
                            pipe(fd);
                            execute(cmdLines, j, NULL, fd, -1);
                        } else
                            execute(cmdLines, j, NULL, NULL, -1);
                        if (cmdLines->blocking)
                            run_job_in_foreground(&jobs, j, 0, shell_attr, shell_pid);
                        else
                            run_job_in_background(j, 0);
                    }
                }
                else
                    keepGoing = 0;
            }
            free_cmd_lines(cmdLines);
        }
        else {
            FREE(shell_attr);
            free_job_list(&jobs);
            perror("gets");
            exit(-1);
        }
    }
    FREE(shell_attr);
    free_job_list(&jobs);
    return 0;
}

void setSignals(){
    if (signal(SIGQUIT, catch_signals) == SIG_ERR)
        debugger("An error occurred while setting a signal handler.");
    if (signal(SIGCHLD, catch_signals) == SIG_ERR)
        debugger("An error occurred while setting a signal handler.");
    if (signal(SIGTSTP, catch_signals) == SIG_ERR)
        debugger("An error occurred while setting a signal handler.");
}

void setSignalsDefault(){
    int err = 0;
    if(signal(SIGTTIN,SIG_DFL) == SIG_ERR) err = 1;
    if(signal(SIGTTOU,SIG_DFL) == SIG_ERR) err = 1;
    if(signal(SIGTSTP, catch_signals) == SIG_ERR) err = 1;
    if(signal(SIGQUIT, catch_signals) == SIG_ERR) err = 1;
    if(signal(SIGCHLD, catch_signals) == SIG_ERR) err = 1;
    if(err == 1)
        debugger("An error occured while setting a signal handler to default");
}

void setIgnoreSignals(){
    int err = 0;
    if(signal(SIGTTIN,SIG_IGN) == SIG_ERR) err = 1;
    if(signal(SIGTTOU,SIG_IGN) == SIG_ERR) err = 1;
    if(signal(SIGTSTP,SIG_IGN) == SIG_ERR) err = 1;
    if(err == 1)
        debugger("An error occured while attempting to set - ignoring a signal");
}

void catch_signals(int signal){
    debugger("setting SIGQUIT signal catcher");
    if(signal == SIGQUIT)
        debugger("caught and now ignoring SIGQUIT");
    debugger("setting SIGTSTP signal catcher");
    if(signal == SIGTSTP)
        debugger("caught and now ignoring SIGSTP");
    debugger("setting SIGCHLD signal catcher");
    if(signal == SIGCHLD)
        debugger("caught and now ignoring SIGCHLD");
}

void printDir(char* buffer){
    debugger("getting current working directory");
    if (getcwd(buffer, PATH_MAX) == NULL) {
        perror("failed to execute getcwd");
        _exit(-1);
    }
    printf("%s:",buffer);
}

int preProcess(cmd_line* cmdLines){
    int ans = 1;
    if(strcmp(cmdLines->arguments[0],"jobs") == 0){
        print_jobs(&jobs);
        ans = 0;
    }
    else if(strcmp(cmdLines->arguments[0],"fg") == 0){
        job* toForeground = find_job_by_index(jobs,atoi(cmdLines->arguments[1]));
        if(toForeground)
            run_job_in_foreground(&jobs,toForeground,1, shell_attr, shell_pid);
        ans = 0;
    }
    else if(strcmp(cmdLines->arguments[0],"bg") == 0){
        job* toBackground = find_job_by_index(jobs,atoi(cmdLines->arguments[1]));
        if(toBackground)
            run_job_in_background(toBackground,1);
        ans = 0;
    }
    return ans;
}

void debugger(char * msg){
    if(debug)
        fprintf(stderr,"%s %s %s\n","*** ",msg," ***");
}

void execute(cmd_line* cmd_line1, job* currJob, int* pipeLeft, int* pipeRight, int first_child){
    if (currJob != NULL)
        tcgetattr(STDIN_FILENO, currJob->tmodes);
    pid_t pid;
    if((pid = fork()) < 0){
        debugger("Failed To Fork");
        exit(1);
    }
    else if(pid == 0) {
        setSignalsDefault();
        if(cmd_line1->idx == 0) {
            first_child = getpid();
            currJob->pgid = first_child;
        }
        setpgid(getpid(),getpid());
        if(pipeRight != NULL && cmd_line1->next) {
            close(pipeRight[0]);
            close(1);
            dup(pipeRight[1]);
            close(pipeRight[1]);
        }
        if(pipeLeft) {
            close(pipeLeft[1]);
            close(0);
            dup(pipeLeft[0]);
            close(pipeLeft[0]);
        }
        if (cmd_line1->input_redirect) {
            close(0);
            if (open(cmd_line1->input_redirect, O_RDONLY, 0444)) {
                perror("Failed to open input file");
                _exit(-1);
            }
        }
        if (cmd_line1->output_redirect) {
            close(1);
            if(open(cmd_line1->output_redirect, O_WRONLY | O_CREAT, 0666)){
                perror("Failed to open output file");
                _exit(-1);
            }
        }
        if(execvp(cmd_line1->arguments[0], cmd_line1-> arguments) < 0){
            perror("Error");
            _exit(1);
        }
        if(execvp(cmd_line1->arguments[0],cmd_line1->arguments) < 0){
            perror("execvp");
            exit(-1);
        }
    }
    else {
        if(cmd_line1->idx == 0) {
            first_child = pid;
            currJob->pgid = first_child;
        }
        setpgid(getpid(),first_child);
        if (pipeLeft)
            close(pipeLeft[0]);
        if (pipeRight)
            close(pipeRight[1]);
        if(cmd_line1->next){
            int newPipe[2];
            pipe(newPipe);
            execute(cmd_line1->next, currJob, pipeRight, newPipe, first_child);
        }
    }
}
