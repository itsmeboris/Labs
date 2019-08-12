/* toy-io.c
* Limited versions of printf and scanf
*
* Programmer: Mayer Goldberg, 2018
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* the states in the printf state-machine */

enum printf_state {
    st_printf_start =0,
    st_printf_init =1,
    st_printf_percent =2,
    st_printf_stop =3,
};

typedef struct {
    char* fs;
    int chars_printed;
    enum printf_state state;
} state_args;


typedef struct{
    int printed_chars;
    enum printf_state new_state;
}state_result;


#define MAX_NUMBER_LENGTH 64
#define is_octal_char(ch) ('0' <= (ch) && (ch) <= '7')

state_result start_state_handler(va_list args, int* handler_printed_chars, state_args* state_args);
state_result init_state_handler(va_list args, int* handler_printed_chars, state_args* state_args);
state_result percent_state_handler(va_list args, int* handler_printed_chars, state_args* state_args);
state_result stop_state_handler(va_list args, int* handler_printed_chars, state_args* state_args);

int print_int_helper(int n, int radix, const char *digit);
int print_int(int n, int radix, const char * digit);
int print_string(char* str);
int toy_printf(char *fs, ...);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

int (*handlers[128])(va_list args, state_args* state_args);

int print_int_helper(int n, int radix, const char *digit) {
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

int print_int(int n, int radix, const char * digit) {
    if (radix < 2 || radix > 16) {
        toy_printf("Radix must be in [2..16]: Not %d\n", radix);
        exit(-1);
    }
    
    if (n > 0) {
        return print_int_helper(n, radix, digit);
    }
    if (n == 0) {
        putchar('0');
        return 1;
    }
    else {
        putchar('-');
        return 1 + print_int_helper(-n, radix, digit);
    }
}

int print_string(char* str){
    int res =0;
    while (*str != '\0') {
        putchar(*(str));
        str += 1;
        res++;
    }
    return res;
}

/**********************************************************************************************************************************************
* SUPPORTED:
*   %b, %d, %o, %x, %X --
*     integers in binary, decimal, octal, hex, and HEX
*   %s -- strings
*********************************************************************************************************************************************/

int default_handler(va_list args, state_args* state_args){
    putchar(*(state_args->fs));
    return 1;
}

int int_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    return print_int(va_arg(args, int), 10, digit);
}

int smallHex_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    return print_int(va_arg(args, int), 16, digit);
}

int largeHex_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    return print_int(va_arg(args, int), 16, DIGIT);
}

int char_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    putchar((char) va_arg(args, int));
    return 1;
}

int oct_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    return print_int(va_arg(args, int), 8, digit);
}

int binary_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    return print_int(va_arg(args, int), 2, digit);
}

int string_handler(va_list args, state_args* state_args){
    if(state_args->state == st_printf_init)
        return default_handler(args,state_args);
    state_args->state = st_printf_init;
    return print_string(va_arg(args, char *));
}

int percent_handler(va_list args, state_args* state_args){
    state_args->state = st_printf_percent;
    return 0;
}


state_result start_state_handler(va_list args, int* handler_printed_chars, state_args* state_args){
    state_result res;
    res.new_state = st_printf_init;
    res.printed_chars = *handler_printed_chars;
    for(int i=0;i < 128; i++)
        handlers[i] = &default_handler;
    handlers['d'] = &int_handler;
    handlers['o'] = &oct_handler;
    handlers['c'] = &char_handler;
    handlers['x'] = &smallHex_handler;
    handlers['X'] = &largeHex_handler;
    handlers['s'] = &string_handler;
    handlers['b'] = &binary_handler;
    handlers['%'] = &percent_handler;
    return res;
}


state_result init_state_handler(va_list args, int* handler_printed_chars, state_args* state_args){
    state_result res;
    res.new_state = st_printf_stop;
    while (*(state_args->fs) != '\0' && res.new_state == st_printf_stop) {
        *handler_printed_chars += handlers[*(state_args->fs)](args, state_args);
        res.new_state = state_args->state;
        (state_args->fs) +=1;
    }
    res.printed_chars = *handler_printed_chars;
    return res;
}

state_result percent_state_handler(va_list args, int* handler_printed_chars, state_args* state_args) {
    state_result res;
    res.new_state = st_printf_init;
    res.printed_chars = handlers[*(state_args->fs)](args, state_args);
    state_args->fs +=1;
    return res;
}


state_result stop_state_handler(va_list args, int* handler_printed_chars, state_args* state_args){
    state_result res;
    res.new_state = st_printf_start;
    res.printed_chars = *handler_printed_chars;
    return res;
}



int toy_printf(char *fs, ...) {
    state_result (*handle[4])(va_list args, int* handler_printed_chars, state_args* state_args) = {&start_state_handler, &init_state_handler,percent_state_handler, &stop_state_handler};
    int printed_chars = 0;
    va_list args;
    state_result res;
    res.new_state = st_printf_start;
    state_args stateArgs;
    stateArgs.chars_printed =0;
    stateArgs.fs = fs;
    va_start(args, fs);
    while(res.new_state != st_printf_stop){
        res = handle[res.new_state](args, &stateArgs.chars_printed, &stateArgs);
        printed_chars += res.printed_chars;
    }
    va_end(args);
    return printed_chars;
}