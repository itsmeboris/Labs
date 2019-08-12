#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include "common.h"
#include "line_parser.h"

typedef struct {
    char *name;
    int (*func)();
} menu;

void initialize();
void closeAll();
int bye(char* const args[], int args_len);
int connectToy(char* const args[], int args_len);
int exec(char* cmd, char* const args[], int args_len);
int ls(char* const args[], int args_len);


int debug = 0;
int shutDown = 0;
char input[2000];
cmd_line* cmd;
client_state* state;
struct sockaddr_in server;
char server_reply[2000];

menu clientFunctions[] = {
        {"conn",connectToy},
        {"bye",bye},
        {"ls", ls},
        {NULL,NULL}
};

int main (int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-d") == 0) debug = 1;
    initialize();
    while(!shutDown){
        printf("server:%s>", state->server_addr);
        fgets(input, 2000, stdin);
        cmd = parse_cmd_lines(input);
        if(strcmp(cmd->arguments[0], "quit") == 0)
            shutDown = 1;
        else {
            if (exec(cmd->arguments[0], cmd->arguments, cmd->arg_count))
                shutDown = 1;
        }
        free_cmd_lines(cmd);
    }
    closeAll();
    return 0;
}

void closeAll(){
    FREE(state->client_id);
    state->client_id = NULL;
    FREE(state->server_addr);
    state->server_addr = "nil";
    state->sock_fd = -1;
    state->conn_state = IDLE;
    free(state);
}


int exec(char* cmd, char* const args[], int args_len){
    int index = 0;
    while (clientFunctions[index].func) {
        if(strcmp(clientFunctions[index].name,cmd) == 0)
            return clientFunctions[index].func(args, args_len);
        index++;
    }
    perror(strcat(cmd, " :no such command exists"));
    return -1;
}

void initialize(){
    state = (client_state*)malloc(sizeof(client_state));
    state->client_id = NULL;
    state->server_addr = "nil";
    state->conn_state = IDLE;
    state->sock_fd = -1;
}

int connectToy(char* const args[], int args_len){
   if(state->conn_state != IDLE)
       return -2;
    if((state->sock_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1){
        perror("failed to open socket");
        return -1;
    }
    debugger(debug, state->server_addr, "Socket created");
    if(strcmp(args[1], "localhost") == 0)
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
    else
        server.sin_addr.s_addr = inet_addr(args[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(2018);
    state->conn_state = CONNECTING;
    if (connect(state->sock_fd , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed");
        return -1;
    }
    debugger(debug, state->server_addr, "Sending Hello");
    if(send(state->sock_fd , "hello\n" , strlen("hello\n") , 0) < 0) {
        perror("Send hello failed");
        return -1;
    }
    debugger(debug, state->server_addr, "waiting for hello");
    if(recv(state->sock_fd , server_reply , 2000 , 0) < 0) {
        perror("recv hello failed");
        return -1;
    }
    debugger(debug, state->server_addr, "received message");
    cmd_line* temp = parse_cmd_lines(server_reply);
    if(temp->arg_count != 2 || strcmp(temp->arguments[0], "hello") != 0) {
        perror("wrong receive message");
        return -1;
    }
    if(strcmp(temp->arguments[0], "nok") == 0){
        fprintf(stderr, "Server Error: %s", temp->arguments[1]);
        return -2;
    }
    debugger(debug, state->server_addr, "Connected");
    state->client_id = malloc(strlen(temp->arguments[1]) + 1);
    strcpy(state->client_id, temp->arguments[1]);
    state->conn_state = CONNECTED;
    state->server_addr = malloc(strlen(args[1]) +1);
    strcpy(state->server_addr, args[1]);
    free_cmd_lines(temp);
    memset(server_reply, '\0', 2000);
    return 0;
}

int ls(char* const args[], int args_len){
    if(state->conn_state != CONNECTED)
        return -2;
    if(send(state->sock_fd , "ls" , strlen("ls") , 0) < 0) {
        perror("Send ls failed");
        return -1;
    }
    if(recv(state->sock_fd , server_reply , 3 , 0) < 0) {
        perror("recv ls failed");
        return -1;
    }
    if(strcmp(server_reply, "ok\n") != 0) {
        memset(server_reply, '\0', 3);
        if(recv(state->sock_fd , server_reply , 1997 , 0) < 0) {
            perror("recv nok error failed");
            return -1;
        }
        fprintf(stderr, "Server Error: %s", server_reply);
        return -1;
    }
    if(recv(state->sock_fd , server_reply , 1997 , 0) < 0) {
        perror("recv ls failed");
        return -1;
    }
    fprintf(stdout, "%s", server_reply);
    memset(server_reply, '\0', 2000);
    return 0;
}

int bye(char* const args[], int args_len){
    if(state->conn_state != CONNECTED)
        return -2;
    if(send(state->sock_fd , "bye" , strlen("bye") , 0) < 0) {
        perror("Send bye failed");
        return -1;
    }
    shutdown(state->sock_fd,2);
    shutDown = 1;
    return 0;
}
