#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "../src/refmem.h"

// Prototypes

static bool not_empty(char *str);

static void clear_input_buffer(void);

bool is_number(char *str);

static bool is_float(char *str);

static answer_t make_float(char *str);

answer_t get_char(char *str);

bool is_char(char *str);

static char *ref_strdup(char *src);

// Public functions

answer_t ask_question(char *question, check_func check, convert_func convert)
{
    char buffert[255] = "";
    int looptrue = 0;

    printf("%s\n", question);

    while (looptrue == 0)
    {
        read_string(buffert, 255);
        if (check(buffert))
        {
            looptrue = 1;
        }
    }

    return convert(buffert);
}

int ask_question_int(char *question)
{
    return ask_question(question, is_number, (convert_func) atoi)
        .int_value;   // svaret som ett heltal
}

double ask_question_float(char *question)
{
    return ask_question(question, is_float, make_float).float_value;
}

char *ask_question_string(char *question)
{
    return ask_question(question, not_empty, (convert_func) ref_strdup).string_value;
}

char ask_question_char(char *question)
{
    return ask_question(question, is_char, get_char).char_value;
}

long ask_question_long(char *question)
{
    return ask_question(question, is_number, (convert_func) atol).long_value;
}

// Private
static bool not_empty(char *str) { return strlen(str) > 0; }

static void clear_input_buffer(void)
{
    int c;
    do
    {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

bool is_number(char *str)
{

    int len = strlen(str);
    for (int i = 0; i < len; i += 1)
    {
        if (isdigit(((unsigned char *)str)[i]) || ((i == 0) && (str[i] == '-') && isdigit(((unsigned char *)str)[i + 1])))
        {
            continue;
        }
        return false;
    }
    return true;
}

bool is_char(char *str)
{
    int len = strlen(str);
    return len == 1 && isprint(((unsigned char *)str)[0]) && !isdigit(((unsigned char *)str)[0]) ? true : false;
}

answer_t get_char(char *str) { return (answer_t){.char_value = str[0]}; }

static bool is_float(char *str)
{
    char *err;
    strtod(str, &err);
    return (*err == '\0');
}

int read_string(char *buf, int buf_siz)
{
    int counter = 0;
    scanf("%[^\n]", buf);
    int len = strlen(buf);
    while (counter < buf_siz - 1 && counter < len)
    {
        counter += 1;
    }

    clear_input_buffer();

    return counter;
}

static answer_t make_float(char *str)
{
    double answer = strtod(str, NULL);
    return (answer_t){.float_value = answer};
}

void print(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        putchar(str[i]);
        i += 1;
    }
}

static char *ref_strdup(char *src)
{
    size_t len = strlen(src);
    char *str = allocate_array(len + 1, sizeof(char), NULL);
    retain(str);
    strcpy(str, src);
    return str;
}
