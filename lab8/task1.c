#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <byteswap.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>

void quit();
void examineElf();
void setFileName();
void printSectionNames();
void printSymbols();
void closemyf();

typedef struct {
    char *name;
    void (*func)();
} menu;


menu menufunctions[] = {
    {"Examine ELF file",examineElf},
    {"Print Section Names",printSectionNames},
    {"Print Symbols",printSymbols},
    {"Quit",quit},
    {NULL,NULL}
};

struct stat fd_stat;
int fd = -1;
void *map_start;
Elf64_Ehdr *header;
char file[100];
char userInput[100];
char* format = "[%2d]: %8x %8d %16s\t%s\n";

void closemyf(){
 if(fd > 0)
        close(fd);
    if(map_start)
        munmap(map_start, fd_stat.st_size);   
}

void quit(){
    closemyf();
    exit(0);
}

void examineElf(){
    closemyf();
    setFileName();
    char magic[3]={header->e_ident[1],header->e_ident[2],header->e_ident[3]};
    printf("%s %s\n","Magic:",magic);
    printf("%s %d\n","Decoding:",header->e_ident[5]);
    printf("%s 0x%x\n","Entry Point:",(int)header->e_entry);
    printf("%s %d\n","Section header offset:",(int)header->e_shoff);
    printf("%s %d\n","Section header entries:",header->e_shnum);
    printf("%s %d\n","Section header entry size:",header->e_shentsize);
    printf("%s %d\n","Program header offset:",(int)header->e_phoff);
    printf("%s %d\n","Program header entries:",header->e_phnum);
    printf("%s %d\n","Program header entry size:",header->e_phentsize);
}

void printSectionNames(){
    setFileName();
    int i;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(map_start + header->e_shoff);
    Elf64_Shdr *sh_strtab = &shdr[header->e_shstrndx];
    const char *const sh_strtab_p = map_start + sh_strtab->sh_offset;
    for (i = 0; i < header->e_shnum; ++i) {
        printf("[%2d] %20s %8x %10d %10d %8x\n", i, sh_strtab_p + shdr[i].sh_name , (int)shdr[i].sh_addr , (int)shdr[i].sh_offset , (int)shdr[i].sh_size , shdr[i].sh_type);
    }
}


void setFileName() {
    if(fd > 0){
        return;
    }
    if(strlen(file) == 0){
        printf("Enter file name\n");
        fgets(file, 100, stdin);
    }
    file[strlen(file)-1] = 0;
    if( (fd = open(file, O_RDWR)) < 0 ) {
        fd=-1;
        perror("error in open");
        exit(-1);
    }
    if( fstat(fd, &fd_stat) != 0 ) {
        fd = -1;
        perror("stat failed");
        exit(-1);
    }
    if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        fd = -1;
        munmap(map_start, fd_stat.st_size);
        perror("mmap failed");
        exit(-1);
    }
    header = (Elf64_Ehdr*) map_start;
}




void printSymbols(){
    setFileName();
    int i,j=0;
    Elf64_Sym* sym=NULL;
    void *symb,*map = map_start;
    Elf64_Shdr* S_hd = (Elf64_Shdr*)(map+header->e_shoff);
    char *inner_str ,*str_tbl = (char*)(map+(int)(S_hd[header->e_shstrndx].sh_offset));
    int counter=0;
    for(i=0;i<=header->e_shnum;i++){
        if(S_hd[i].sh_type==SHT_SYMTAB || S_hd[i].sh_type==SHT_DYNSYM){
            if(S_hd[i].sh_type==SHT_DYNSYM && counter == 0)
                printf("\nSymbol table '.dynsym':\n\n");
            if(S_hd[i].sh_type==SHT_SYMTAB){
                counter = 0;
                printf("\nSymbol table '.symtab':\n");
            }
            symb=map+S_hd[i].sh_offset;
            for(j=0;j<(S_hd[i].sh_size/S_hd[i].sh_entsize);j++){
                sym=symb;
                inner_str = map+(S_hd[S_hd[i].sh_link].sh_offset);
                if(sym->st_shndx!= SHN_ABS)
                {
                    if(sym->st_shndx!= SHN_UNDEF)
                        printf(format,counter,(int)sym->st_value,sym->st_shndx,str_tbl+S_hd[sym->st_shndx].sh_name,inner_str+sym->st_name);
                    else
                        printf(format,counter,(int)sym->st_value,"UND",str_tbl+S_hd[sym->st_shndx].sh_name,inner_str+sym->st_name);
                }
                else
                    printf(format,j,(int)sym->st_value,"ABC", " " ,inner_str+sym->st_name);
                symb+=S_hd[i].sh_entsize;
                counter++;
            }
        }
    }
}

int main(int argc, char **argv) {
    int index;
    int selectedFunc = -1;
    while (1) {
        printf("Choose action:\n");
        index = 0;
        while (menufunctions[index].func) {
            printf("%d - %s\n", index + 1, menufunctions[index].name);
            index++;
        }
        selectedFunc = atoi(fgets(userInput, 100, stdin)) - 1;
        if (selectedFunc >= 0 && selectedFunc < index)
            menufunctions[selectedFunc].func();
    }
}