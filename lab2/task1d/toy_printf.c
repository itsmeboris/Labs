/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

/* the states in the printf state-machine */
enum printf_state {
    st_printf_init,
    st_printf_percent,
    st_printf_array,
    st_printf_space
};

#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')
int toy_printf(char *fs, ...);
int len(int num);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int print_int_helper(unsigned int n, int radix, const char *digit) {
    int result;

    if (n < radix) {
        putchar(digit[n]);
        return 1;
    }
    else {
        result = print_int_helper(n / radix, radix, digit);
        putchar(digit[n % radix]);
        return 1 + result;
    }
}

void print_int_padding(int n, int radix, const char *digit, int size, int beforeNum, int zero) {
    int length = len(n);
    if(length >= size) {
        if(n < 0){
            putchar('-');
            n = (-1)*n;
        }
        print_int(n,radix,digit);
    }
    else{
        if(!beforeNum){
            if(n < 0){
                putchar('-');
                n = (-1)*n;
            }
            print_int(n,radix,digit);
            while (length <= size) {
                putchar(' ');
                length++;
            }
            putchar('#');
        }
        else{
            if(zero){
                if(n < 0) {
                    putchar('-');
                    n = (-1)*n;
                    length++;
                }
                while(length <= size){
                    putchar('0');
                    length++;
                }
                print_int(n,radix,digit);
            }
            else{
                while(length <= size){
                    putchar(' ');
                    length++;
                }
                if(n < 0){
                    putchar('-');
                    n = (-1)*n;
                }
                print_int(n,radix,digit);
            }
        }
    }
}

int print_int(unsigned int n, int radix, const char * digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }

    if (n > 0) {
        return print_int_helper(n, radix, digit);
    }
    else if (n == 0) {
        putchar('0');
        return 1;
    }
    else {
        return print_int_helper(n, radix, digit);
    }
}

/* SUPPORTED:
 *   %b, %d, %o, %x, %X -- 
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

int toy_printf(char *fs, ...) {
    int chars_printed = 0;
    int int_value = 0;
    char *string_value;
    char char_value;
    int *numArr;
    char **charArr;
    int beforeNum = 0;
    int zero = 0;
    int size = 0;
    int width = 0;
    va_list args;
    va_list arr;
    enum printf_state state;

    va_start(args, fs);

    state = st_printf_init;

    for (; *fs != '\0'; ++fs) {
        switch (state) {
            case st_printf_init:
                switch (*fs) {
                    case '%':
                        state = st_printf_percent;
                        break;

                    default:
                        putchar(*fs);
                        ++chars_printed;
                }
                break;

            case st_printf_percent:
                switch (*fs) {
                    case '%':
                        putchar('%');
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case 'd':
                        int_value = va_arg(args, int);
                        if(int_value < 0){
                            putchar('-');
                            chars_printed++;
                            int_value = (-1)*int_value;
                        }
                        chars_printed += print_int(int_value, 10, digit);
                        state = st_printf_init;
                        break;

                    case 'b':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 2, digit);
                        state = st_printf_init;
                        break;

                    case 'o':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 8, digit);
                        state = st_printf_init;
                        break;

                    case 'x':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 16, digit);
                        state = st_printf_init;
                        break;

                    case 'X':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 16, DIGIT);
                        state = st_printf_init;
                        break;

                    case 's':
                        string_value = va_arg(args, char *);
                        chars_printed += printString(string_value);
                        state = st_printf_init;
                        break;

                    case 'c':
                        char_value = (char)va_arg(args, int);
                        putchar(char_value);
                        ++chars_printed;
                        state = st_printf_init;
                        break;

                    case 'u':
                        int_value = va_arg(args, int);
                        chars_printed += print_int(int_value, 10, DIGIT);
                        state = st_printf_init;
                        break;

                    case 'A':
                        state = st_printf_array;
                        break;

                    default:
                        if(*fs == '-' || (*fs <= '9' && *fs >= '0')) {
                            state = st_printf_space;
                            fs--;
                        }
                        else {
                            toy_printf("Unhandled format %%%c...\n", *fs);
                            exit(-1);
                        }
                }
                break;

            case st_printf_array:
                putchar('{');
                ++chars_printed;
                switch (*fs) {
                    case 'd':
                        numArr = va_arg(args, int *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            if(numArr[i] < 0){
                                putchar('-');
                                chars_printed++;
                                numArr[i] = (-1)*numArr[i];
                            }
                            chars_printed += print_int(numArr[i], 10, digit);
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        if(numArr[size - 1] < 0){
                            putchar('-');
                            chars_printed++;
                            numArr[size - 1] = (-1)*numArr[size - 1];
                        }
                        chars_printed += print_int(numArr[size-1], 10, digit);
                        break;

                    case 'b':
                        numArr = va_arg(args, int *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            chars_printed += print_int(numArr[i], 2, digit);
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        chars_printed += print_int(numArr[size-1], 2, digit);
                        break;

                    case 'o':
                        numArr = va_arg(args, int *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            chars_printed += print_int(numArr[i], 8, digit);
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        chars_printed += print_int(numArr[size-1], 8, digit);
                        break;

                    case 'x':
                        numArr = va_arg(args, int *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            chars_printed += print_int(numArr[i], 16, digit);
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        chars_printed += print_int(numArr[size-1], 16, digit);
                        break;

                    case 'X':
                        numArr = va_arg(args, int *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            chars_printed += print_int(numArr[i], 16, DIGIT);
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        chars_printed += print_int(numArr[size-1], 16, DIGIT);
                        break;

                    case 's':
                        charArr = va_arg(args, char **);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            putchar('"');
                            ++chars_printed;
                            string_value = charArr[i];
                            chars_printed += printString(string_value);
                            putchar('"');
                            ++chars_printed;
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        string_value = charArr[size - 1];
                        putchar('"');
                        ++chars_printed;
                        chars_printed += printString(string_value);
                        putchar('"');
                        ++chars_printed;
                        break;

                    case 'c':
                        string_value = va_arg(args, char *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            putchar('\'');
                            putchar(string_value[i]);
                            putchar('\'');
                            chars_printed += 3;
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        putchar('\'');
                        putchar(string_value[size - 1]);
                        putchar('\'');
                        ++chars_printed;
                        break;

                    case 'u':
                        numArr = va_arg(args, int *);
                        size = va_arg(args,int);
                        if(size <= 0) break;
                        for(int i=0;i<size - 1;i++){
                            chars_printed += print_int(numArr[i], 10, digit);
                            putchar(',');
                            putchar(' ');
                            chars_printed += 2;
                        }
                        chars_printed += print_int(numArr[size-1], 10, digit);
                        break;
                }
                putchar('}');
                ++chars_printed;
                state = st_printf_init;
                break;
            case st_printf_space:
                switch (*fs){
                    case '-':
                        beforeNum = 1;
                        break;

                    case '0':
                        beforeNum = 1;
                        zero = 1;
                        break;

                    default:
                        if(*fs <= '9' && *fs >= '0'){
                            width = width*10 + fs[0]- '0';
                        }
                        else if(*fs == 'd'){
                            int_value = va_arg(args,int);
                            print_int_padding(int_value,10,digit,width,beforeNum,zero);
                            state = st_printf_init;
                            zero =0;
                            beforeNum = 0;
                            width =0;
                        }
                        else if(*fs == 's'){
                            string_value = va_arg(args,char *);
                            int length = strlen(string_value);
                            if(length < width){
                                if(beforeNum){
                                    while(length <= width) {
                                        putchar(' ');
                                        length++;
                                    }
                                    chars_printed += printString(string_value);
                                }
                                else{
                                    chars_printed += printString(string_value);
                                    while(length <= width) {
                                        putchar(' ');
                                        length++;
                                    }
                                    putchar('#');
                                }
                            }
                            else{
                                chars_printed += printString(string_value);
                            }
                            state = st_printf_init;
                            zero =0;
                            beforeNum = 0;
                            width =0;
                        }
                        else{
                            toy_printf("Unhandled format %%%c...\n", *fs);
                            exit(-1);
                        }
                        break;
                }
                break;
        }
    }

    va_end(args);

    return chars_printed;
}

int len(int num){
    if(num < 0) return 1 + len((-1)*num);
    else if(num < 10) return 1;
    return 1 + len(num/10);
}

int printString(char *string_value){
    int res = 0;
    while(*string_value){
        res++;
        putchar(*string_value);
        string_value++;
    }
    return res;
}


