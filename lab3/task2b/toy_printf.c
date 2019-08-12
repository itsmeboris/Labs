/* toy-io.c
 * Limited versions of printf and scanf
 *
 * Programmer: Mayer Goldberg, 2018
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* the states in the printf state-machine */

typedef struct {
    char* fs;
    int chars_printed;
    int int_value;
    char *string_value;
    char char_value;
} state_args;

enum printf_state {
    st_printf_start =0,
    st_printf_init =1,
    st_printf_percent =2,
    st_printf_stop =3,
};

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
int toy_printf(char *fs, ...);

void reset(state_args* state);

const char *digit = "0123456789abcdef";
const char *DIGIT = "0123456789ABCDEF";

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

/* SUPPORTED:
 *   %b, %d, %o, %x, %X --
 *     integers in binary, decimal, octal, hex, and HEX
 *   %s -- strings
 */

state_result init_state_handler(va_list args, int* handler_printed_chars, state_args* state_args){
    reset(state_args);
    state_result res;
    res.new_state = st_printf_stop;
    for (; *(state_args->fs) != '\0' && res.new_state == st_printf_stop; (state_args->fs) +=1) {
        switch (*(state_args->fs)) {
            case '%':
                res.new_state = st_printf_percent;
                break;

            default:
                putchar(*(state_args->fs));
                *handler_printed_chars +=1;
        }
    }
    res.printed_chars = *handler_printed_chars;
    return res;
}

state_result percent_state_handler(va_list args, int* handler_printed_chars, state_args* state_args) {
    reset(state_args);
    state_result res;
    res.new_state = st_printf_start;
    switch (*(state_args->fs)) {
        case 'd':
            state_args->int_value = va_arg(args, int);
            *handler_printed_chars += print_int(state_args->int_value, 10, digit);
            break;

        case 'b':
            state_args->int_value = va_arg(args, int);
            *handler_printed_chars += print_int(state_args->int_value, 2, digit);
            break;

        case 'o':
            state_args->int_value = va_arg(args, int);
            *handler_printed_chars += print_int(state_args->int_value, 8, digit);
            break;

        case 'x':
            state_args->int_value = va_arg(args, int);
            *handler_printed_chars += print_int(state_args->int_value, 16, digit);
            break;

        case 'X':
            state_args->int_value = va_arg(args, int);
            *handler_printed_chars += print_int(state_args->int_value, 16, DIGIT);
            break;

        case 's':
            state_args->string_value = va_arg(args, char *);
            while (*state_args->string_value) {
                *handler_printed_chars +=1;
                putchar(*(state_args->string_value));
                (state_args->string_value) += 1;
            }
            break;

        case 'c':
            state_args->char_value = (char) va_arg(args, int);
            putchar(state_args->char_value);
            *handler_printed_chars +=1;
            break;

        default:
            toy_printf("Unhandled format %%%c...\n", *(state_args->fs));
            exit(-1);
    }
    state_args->fs +=1;
    res.printed_chars =  *handler_printed_chars;
    return res;
}

state_result start_state_handler(va_list args, int* handler_printed_chars, state_args* state_args){
    reset(state_args);
    state_result res;
    res.new_state = st_printf_init;
    res.printed_chars = *handler_printed_chars;
    return res;
}

state_result stop_state_handler(va_list args, int* handler_printed_chars, state_args* state_args){
    reset(state_args);
    state_result res;
    res.new_state = st_printf_start;
    res.printed_chars = *handler_printed_chars;
    return res;
}

void reset(state_args* state){
    state->chars_printed =0;
}



int toy_printf(char *fs, ...) {
    state_result (*handle[4])(va_list args, int* handler_printed_chars, state_args* state_args) = {&start_state_handler, &init_state_handler,percent_state_handler, &stop_state_handler};;
    int printed_chars = 0;
    va_list args;
    state_result res;
    res.new_state = st_printf_init;
    state_args state_args;
    state_args.fs = fs;
    va_start(args, fs);
    while(res.new_state != st_printf_stop){
        res = handle[res.new_state](args, &state_args.chars_printed, &state_args);
        printed_chars += state_args.chars_printed;
    }
    va_end(args);
    return printed_chars;
}