#pragma once

#include <stdbool.h>

typedef union
{
    int int_value;
    long long_value;
    double float_value;
    char *string_value;
    char char_value;
} answer_t;

typedef bool (*check_func)(char *);

typedef answer_t (*convert_func)(char *);

bool is_number(char *str);

answer_t ask_question(char *question, check_func check, convert_func convert);

int ask_question_int(char *question);

char *ask_question_string(char *question);

double ask_question_float(char *question);

long ask_question_long(char *question);

char ask_question_char(char *question);

int read_string(char *buf, int buf_siz);

void print(char *str);
