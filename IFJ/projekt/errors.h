/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */

#ifndef IFJ_ERRORS_H
#define IFJ_ERRORS_H

#define SYN_OK 0

#define ERR_LEX             1 // chybna struktura lexemu
#define ERR_SYN             2 // chybna syntaxe programu
#define ERR_SEM             3 // nedef. fce/prom, pokus o redef. fce/prom, atp.
#define ERR_SEM_TYPE        4 // nekompatibilita v aritmetickych, retezcovych a relacnich vyrazech
#define ERR_SEM_PARAM       5 // spatny pocet parametru u volani funkce
#define ERR_SEM_OTHERS      6 // ostatni semanticke chyby
#define ERR_ZERO_DIVISION   9 // behova chyba deleni nulou
#define ERR_INTERN          99 // interni chyba prekladace - alokace pameti, neuspech otevreni souboru, ..


#endif //IFJ_ERRORS_H
