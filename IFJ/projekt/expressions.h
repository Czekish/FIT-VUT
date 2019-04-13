/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */


#ifndef IFJ_EXPRESSIONS_H
#define IFJ_EXPRESSIONS_H

#include "str.h"
#include <stdio.h>

#define MUL_PREC             0
#define DIV_PREC             1
#define ADD_PREC             2
#define SUB_PREC             3
#define LESS_PREC            4
#define GREAT_PREC           5
#define LESS_EQL_PREC        6
#define GREAT_EQL_PREC       7
#define EQL_PREC             8
#define NOT_EQL_PREC         9
#define LEFT_BRACKET_PREC   10
#define RIGHT_BRACKET_PREC  11
#define ID_PREC             12
#define DOLLAR_PREC         13
#define ASSIGN_PREC         14


void string_concat(string *result, string *to_concate);
//int reduce(struct Stack* stack);
//int get_str_expression(string *first_string);
int get_expression(char* firstID, int token, string attr, char *variabile);
 int get_if_or_while_expression(int then_or_do, int token, string attr, int random); 


#endif //IFJ_EXPRESSIONS_H
