/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */

#include "scanner.h"
#include "errors.h"
#include "str.h"
#include "symtable.h"
#include "generate.h"
#include "stack.h"
#include "expressions.h"

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

 /**
  * Funkcia zistuje podla indexu do TS ci dana premenna alebo funkcia je definovana.
  */
 bool isDeclared();
 /**
  * Pomocna funkcia v ktorej sa nasledne volaju funkcie na uvolnenie dynamicky alokovanej
  * pamate za behu programu.
  */
 int freeAll();

 int statList();
 int stat();
 int itemList();
 int itemList2();
 int item();
#endif //IFJ_PARSER_H
