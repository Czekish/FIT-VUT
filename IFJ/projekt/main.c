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
#include <stdlib.h>

#include "scanner.h"
#include "errors.h"
#include "parser.h"
#include "str.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    /**
    // Osetreni vstupnich parametru
    if(argc != 2) {
        fprintf(stderr, "Chyba vstupnich parametru!");
        return ERR_INTERN;
    }

    // otevreni souboru
    FILE *f;

    if((f = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Chyba pri otevirani vstupniho souboru!");
        return ERR_INTERN;
    }

    setSourceFile(f);
    */
    // todo inicializace tabulky symbolu

    // todo inicializace seznamu instrukci

    int result = statList();
   // printf("%d\n",result);

    // Uzavreni souboru
    //closeSourceFile();
    //close(f);

    return result;
}
