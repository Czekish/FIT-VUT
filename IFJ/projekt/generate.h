/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "errors.h"

#define FUNC_TABLE 0
#define LOCAL_V_TABLE 1
#define NO_TS 2

FILE *output;

/**
 * Funkcia zistuje ci je zadany string Integer.
 */
bool isInt();
/**
 * Funckia zistuje ci je zadany string Float.
 */
bool isFloat();
/**
 * 123 => 321
 */
int reverseNumber();
/**
 * Funckia generuje string vo formate spravnom pre jazyk ifj18int.
 * abcd\n => abcd\010
 */
void toRightString();
/**
 * Funkcia vyuzivajuca funkcie isInt a isFloat zistuje typ danej premennej.
 */
int getType();

void Generate_start_file();
void Generate_func_start();
void Generate_func_end();
void Pass_func_parametres();
void Generate_assign_func_var();
void Generate_to_func_var();
void Generate_assign_var();
void Generate_to_var();
int Generate_built_in();
void Generate_read();
void Generate_push_number(char *val_str);
void Generate_push_string(char *val_str);
void Generate_final_move_int_var(char *variabile);
void Generate_stack_dividing(int random);
void Generate_stack_adds(int random);
void Generate_stack_muls(int random);
void Generate_stack_subs(int random);
void Generate_push_LF(char *print);
/// Funkce pro generovani v IF
void Generate_less_great(int random, int less_or_great);
void Generate_less_great_eq(int random, int rel_op);
void Generate_eq(int random, int rel_op);
