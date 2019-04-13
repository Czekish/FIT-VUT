/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H


// STAVY
#define INIT_STATE              10
#define ID_STATE                11
#define LESS_STATE              12
#define GREATER_STATE           13
#define EXCLAMATION_STATE       14
#define NUMBER_STATE            15
#define DOT_STATE               16
#define EXP_STATE               17
#define DOT_PART_STATE          18
#define SIGN_STATE              19
#define EXP_PART_STATE          20
#define STRING_STATE            21
#define ESCAPE_STATE            22
#define HEX_STATE               23
#define HEX_NUMBER_STATE        24
#define LINE_COMM_STATE         25
#define EQUAL_STATE             26
#define NEW_LINE_STATE          27
#define BLOCK_COMM_STATE        28
#define BLOCK_COMM_END_STATE    29

// TOKENY
#define LEFT_BRACKET            50
#define RIGHT_BRACKET           51
#define OPER_PLUS               52
#define OPER_MINUS              53
#define OPER_MUL                54
#define OPER_DIV                55
#define GREATER_OR_EQUAL        56
#define NOT_EQUAL               57
#define LESS                    58
#define LESS_OR_EQUAL           59
#define GREATER                 60
#define COMPARE                 61
#define COMMA                   62
#define ID                      63
#define NUMBER                  64
#define STRING                  65
#define ASSIGN                  66
#define EOL                     67

// KLUCOVE SLOVA
#define DEF                     70
#define DO                      71
#define ELSE                    72
#define END                     73
#define IF                      74
#define NOT                     75
#define NIL                     76
#define THEN                    77
#define WHILE                   78
// VESTAVENE FUNKCE
#define INPUTS                  80
#define INPUTI                  81
#define INPUTF                  82
#define PRINT                   83
#define LENGTH                  84
#define SUBSTR                  85
#define ORD                     86
#define CHR                     87

// OSTATNI
#define END_OF_FILE             90

#include "str.h"

void setSourceFile(FILE *f);
void closeSourceFile();
int getNextToken(string *attr, string *begin_str);

#endif //IFJ_SCANNER_H
