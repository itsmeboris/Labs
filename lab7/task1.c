#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define buffer 100

typedef unsigned char byte;

void setName();

void display();

void setSize();

void modify();

void copy();

void quit();

typedef struct {
    char *name;
    void (*func)();
} menu;

menu menuf[] = {
    {"Set File Name", setName},
    {"Set Unit Size", setSize},
    {"File Display",  display},
    {"file Modify", modify},
    {"Copy From File", copy},
    {"Quit",          quit},
    {NULL, NULL}
};

char filename[buffer];
int size = 1;
char input[buffer];

void setName() {
    printf("Enter file name\n");
    fgets(filename, buffer, stdin);
}

void printHex(int length, byte *fileData){
    printf("%s:\n", "Hex");
    for(int i =0; i< length; i++){
        switch (size){
            case 1:
                printf("%02X ", (unsigned char)((char*)(*(&fileData)))[i]);
                break;
            case 2:
                printf("%04X ", (unsigned short)((short*)(*(&fileData)))[i]);
                break;
            case 4:
                printf("%08X ", (unsigned int)((int*)(*(&fileData)))[i]);
                break;
        }
    }
    printf("\n");
}

void printDec(int length, byte *fileData) {
    int i, j;
    printf("%s:\n", "Decimal");
    for (i = 0; i < length; i++) {
        switch (size){
            case 1:
                printf("%u ",(unsigned char)((char*)(*(&fileData)))[i]);
                break;
            case 2:
                printf("%u ",(unsigned short)((short*)(*(&fileData)))[i]);
                break;
            case 4:
                printf("%u ",(unsigned int)((int*)(*(&fileData)))[i]);
                break;
        }
    }
    printf("\n");
}

void display() {
    if (!filename)
        printf("filename is null\n");
    else {
        FILE *fd;
        char * name = strtok(filename,"\n");
        fd = fopen(name, "r");
        if (fd == NULL)
            printf("%s\n", "Failed to open the file");
        else {
            printf("%s\n", "Please enter <location> <length>");
            fgets(input, buffer, stdin);
            const char s[1] = " ";
            char *token;
            token = strtok(input, s);
            int location = strtol(token, NULL, 16);
            token = strtok(NULL, s);
            int length = atoi(token);
            byte *fileData = NULL;
            fileData = malloc(sizeof(char) * length * size);
            fseek(fd, location, SEEK_SET);
            fread((void *) fileData,1, length * size, fd);
            printHex(length, fileData);
            printDec(length, fileData);
            free(fileData);
            fclose(fd);
        }
    }
}

void setSize() {
    int sizeTemp;
    printf("Enter size unit\n");
    scanf("%d", &sizeTemp);
    getc(stdin);
    if ((sizeTemp != 1) && (sizeTemp != 2) && (sizeTemp != 4))
        printf("Error in size input'\n'");
    else
        size = sizeTemp;
}

void modify(){
    if (!filename)
        printf("filename is null\n");
    else{
        FILE *fd;
        fd = fopen(strtok(filename,"\n"), "r+");
        if (fd == NULL)
            printf("%s\n", "Failed to open the file");
        else {
            printf("%s\n", "Please enter <location> <val>");
            fgets(input, buffer, stdin);
            const char s[1] = " ";
            char *token;
            token = strtok(input, s);
            int location = strtol(token, NULL, 16);
            token = strtok(NULL, s);
            int val = strtol(token, NULL, 16);
            fseek(fd, location, SEEK_SET);
            fwrite(&val, 1, size, fd);
            fclose(fd);
        }
    }
}

void copy(){
    if (!filename)
        printf("filename is null\n");
    else{
        FILE *dest;
        dest = fopen(strtok(filename,"\n"), "r+");
        if (dest == NULL)
            printf("%s\n", "Failed to open the file for destination");
        else {
            printf("%s\n", "Please enter <src_file> <src_offset> <dst_offset> <length>");
            fgets(input, buffer, stdin);
            const char s[1] = " ";
            char *token;
            token = strtok(input, s);
            FILE *source = fopen(token, "r");
            if (source == NULL)
                printf("%s\n", "Failed to open the source file");
            else {
                token = strtok(NULL, s);
                int sourceLocation = strtol(token, NULL, 16);
                token = strtok(NULL, s);
                int destLocation = strtol(token, NULL, 16);
                token = strtok(NULL, s);
                int length = atoi(token);
                fseek(dest, destLocation, SEEK_SET);
                fseek(source, sourceLocation, SEEK_SET);
                byte *fileData = malloc(sizeof(char) * length);
                fread((void *) fileData,1, length, source);
                fwrite(fileData, 1, length, dest);
                fclose(dest);
                fclose(source);
                free(fileData);
                printf("Copied %d bytes into FROM %s at %x TO test at %x\n", length, strtok(input, s), sourceLocation,  destLocation);
            }
        }
    }
}

void quit() { exit(0); }

int main(int argc, char **argv) {
    int index;
    int selectedFunc = -1;
    while (1) {
        printf("Choose action:\n");
        index = 0;
        while (menuf[index].func) {
            printf("%d - %s\n", index + 1, menuf[index].name);
            index++;
        }
        selectedFunc = atoi(fgets(input, buffer, stdin)) - 1;
        if (selectedFunc >= 0 && selectedFunc < index)
            menuf[selectedFunc].func();
    }
}