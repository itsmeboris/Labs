#include "toy_stdio.h"

int main(int argc, char *argv[]) {
    int chars_printed = toy_printf("%x, %X\n", 496351, 496351);
    chars_printed += toy_printf("Welcome to \c\n");
    chars_printed += toy_printf("Support for explicit\n");
    toy_printf("Printed %d chars\n", chars_printed);
}