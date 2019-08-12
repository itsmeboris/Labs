#include "toy_stdio.h"

int main(int argc, char *argv[]) {
  int chars_printed = toy_printf("Hex unsigned: %x\n", -1);
  chars_printed += toy_printf("Octal unsigned: %o\n", -1);
  chars_printed += toy_printf("Decimal unsigned: %d\n", -1);
  chars_printed += toy_printf("Unsigned value: %u\n", 15);
  chars_printed += toy_printf("Unsigned value: %u\n", -15);
  int integers_array[] = {-1,2,3,4,5};
  int integers_empty[] = {};
  char * strings_array[] = {"This", "is", "array", "of", "strings"};
  char char_array[] = {'b','o','r','i','s'};
  char empty_array[] = {};
  int array_size = 5;
  chars_printed += toy_printf("Print array of Octal: %Ao\n", integers_array, array_size);
  chars_printed += toy_printf("Print array of integers: %Ad\n", integers_array, array_size);
  chars_printed += toy_printf("Print array of integers: %Ad\n", integers_empty, 0);
  chars_printed += toy_printf("Print array of strings: %As\n", strings_array, array_size);
  chars_printed += toy_printf("Print array of strings: %As\n", empty_array, 0);
  chars_printed += toy_printf("Print array of chars: %Ac\n", char_array,array_size);
  chars_printed += toy_printf("Non-padded int: %s\n", "str");
  chars_printed += toy_printf("Right-padded int: %6d\n", -5);
  chars_printed += toy_printf("Left-added int: %-6d\n", -5);
  chars_printed += toy_printf("Left-added int: %08d\n", -5);
  chars_printed += toy_printf("Right-padded string: %6s\n", "str");
  chars_printed += toy_printf("Left-added string: %-6s\n", "str");
  toy_printf("Printed %d chars\n", chars_printed); 
}

