#define _BSD_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include "common.h"
#include "line_parser.h"

typedef struct {
    char *name;
    int (*func)();
} menu;

void initializeServer();
int helloServer(char* const args[], int args_len);
int byeServer(char* const args[], int args_len);
int lsServer(char* const args[], int args_len);
int execServer(char* cmd, char* const args[], int args_len);
void closeServer();

menu serverFunctions[] = {
        {"hello",helloServer},
        {"bye",byeServer},
        {"ls", lsServer},
        {NULL,NULL}
};

int servDebug = 0;
int bound = 0;
int serverSocket, c, client_sock;
client_state* clientState;
char hostname[256];
struct sockaddr_in server , client;
char client_message[2000];
int shutDown =0;

int main (int argc, char **argv) {
    gethostname(hostname, 256);
    if (argc > 1 && strcmp(argv[1], "-d") == 0) servDebug = 1;
    fprintf(stdout, "Waiting for incoming connections...\n");
    debugger(servDebug, hostname,"Connection accepted");
    while(!shutDown){
        if(!bound)
            initializeServer();
        clientState = (client_state *) malloc(sizeof(client_state));
        clientState->client_id = NULL;
        clientState->server_addr = hostname;
        clientState->conn_state = IDLE;
        clientState->sock_fd = -1;
        c = sizeof(struct sockaddr_in);
        if((client_sock = accept(serverSocket, (struct sockaddr *)&client, (socklen_t*)&c)) < 0){
            perror("accept failed");
            exit(-1);
        }
        while(recv(client_sock , client_message , 2000 , 0) > 0) {
            cmd_line* cmd = parse_cmd_lines(client_message);
            if(execServer(cmd->arguments[0], cmd->arguments, cmd->arg_count) < 0){
                FREE(clientState);
                free_cmd_lines(cmd);
                memset(client_message, '\0', 2000);
                break;
                
            }
            free_cmd_lines(cmd);
            memset(client_message, '\0', 2000);
        }
    }
    return 0;
}


void initializeServer() {
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(2018);
    serverSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (serverSocket == -1) {
        perror("Could not create socket");
        exit(-1);
    }
    debugger(servDebug, hostname,"Socket created");
    if(!bound && bind(serverSocket,(struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("bind failed. Error");
        exit(-1);
    }
    bound = 1;
    debugger(servDebug, hostname, "bind done starting to listen");
    listen(serverSocket , 1);
}


int execServer(char* cmd, char* const args[], int args_len){
    int index = 0;
    while (serverFunctions[index].func) {
        if(strcmp(serverFunctions[index].name,cmd) == 0)
            return serverFunctions[index].func(args, args_len);
        index++;
    }
    fprintf(stderr, "%s|ERROR: Unknown message %s", clientState->client_id, cmd);
    return -1;
}

int helloServer(char* const args[], int args_len){
    if(clientState->conn_state != IDLE) {
        send(client_sock, "nok state\n", strlen("nok state\n"), 0);
        closeServer();
        return -1;
    }
    clientState->conn_state = CONNECTED;
    clientState->sock_fd = client_sock;
    clientState->client_id = "Stav";
    if(send(client_sock , "hello Stav" , strlen("hello Stav") , 0) < 0) {
        perror("Send hello failed");
        return -1;
    }
    fprintf(stdout, "Client %s connected\n", clientState->client_id);
    return 0;
}

int lsServer(char* const args[], int args_len){
    if(clientState->conn_state != CONNECTED) {
        send(client_sock, "nok state\n", strlen("nok state\n"), 0);
        closeServer();
        return -1;
    }
    char* fileList = list_dir();
    if(fileList){
        send(client_sock, "ok\n", strlen("ok\n"), 0);
        send(client_sock,fileList,strlen(fileList), 0);
        char* dirnName = getcwd(NULL,0);
        fprintf(stdout, "Listed files at %s\n", dirnName);
        FREE(fileList);
        FREE(dirnName);
        return 0;
    }
    return -1;
}

int byeServer(char* const args[], int args_len){
    if(clientState->conn_state != CONNECTED) {
        send(client_sock, "nok state\n", strlen("nok state\n"), 0);
        closeServer();
        return -1;
    }
    fprintf(stdout, "Client %s disconnected\n", clientState->client_id);
    closeServer();
    return -1;
}

void closeServer(){
    shutdown(clientState->sock_fd,2);
}
