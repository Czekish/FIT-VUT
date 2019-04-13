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
#include <ctype.h>
#include <stdlib.h>

#include "str.h"
#include "scanner.h"
#include "errors.h"


// promenna pro ulozeni vstupniho souboru
//FILE *source;

//void setSourceFile(FILE *f) {
//    source = f;
//}

//void closeSourceFile(){
//    source = NULL;
//}


//funkcia na prevod hexa cisla na decimalne
void hex_to_int(char *hex_string, int *number) {
    *number = (int)strtol(hex_string, NULL, 16);
}

// hlavni funkce lexikalniho analyzatoru
int getNextToken(string *attr, string *begin_str) {

    int state = INIT_STATE;
    int c; //premenna do ktorej sa ulozi nacitany znak

    strClear(attr); // vymazani obsahu atributu - ukladame nazev atributu
    strClear(begin_str); // vymazani obsahu atributu - pomocny retezec pro blokove komentare

    int zero_counter = 0; // pocitadlo nul
    int num_err = 0; // error

    char hexa[3]; // pomocne pole pro ulozeni hexadecimalnich cisel

    int new_line = 0;



    while (1) {
        c = getchar();
        switch (state) {
            case INIT_STATE:
                if (c == '\n') {
                    state = NEW_LINE_STATE;
                } else if (isspace(c)) {
                    state = INIT_STATE;
                } else if (c == '(') {
                    return LEFT_BRACKET;
                } else if (c == ')') {
                    return RIGHT_BRACKET;
                } else if (c == '+') {
                    return OPER_PLUS;
                } else if (c == '-') {
                    return OPER_MINUS;
                } else if (c == '*') {
                    return OPER_MUL;
                } else if (c == '/') {
                    return OPER_DIV;
                } else if (c == ',') {
                    return COMMA;
                } else if (c == EOF) {
                    return END_OF_FILE;
                } else if ((islower(c)) || (c == '_')) {
                    state = ID_STATE;
                    strAddChar(attr, c);
                } else if (c == '<') {
                    state = LESS_STATE;
                } else if (c == '>') {
                    state = GREATER_STATE;
                } else if (c == '!') {
                    state = EXCLAMATION_STATE;
                } else if (isdigit(c)) {
                    state = NUMBER_STATE;
                    strAddChar(attr, c);
                    if (c == '0') {
                        zero_counter++;
                    }
                } else if (c == '"') {
                    state = STRING_STATE;
                } else if (c == '#') {
                    state = LINE_COMM_STATE;
                } else if (c == '=') {
                    state = EQUAL_STATE;
                }
                else
                  return ERR_LEX;
                break;


                case ID_STATE:
                if ((isalnum(c)) || (c == '_')) {
                    strAddChar(attr, c);
                    state = ID_STATE;
                } else if ((c == '?') || (c == '!')) {
                    strAddChar(attr, c);
                    return ID;
                } else {
                    ungetc(c, stdin);

                    //porovnanie ci nejde o klucove slovo
                    if (strCmpConstStr(attr, "def") == 0) {
                        return DEF;
                    } else if (strCmpConstStr(attr, "do") == 0) {
                        return DO;
                    } else if (strCmpConstStr(attr, "else") == 0) {
                        return ELSE;
                    } else if (strCmpConstStr(attr, "end") == 0) {
                        return END;
                    } else if (strCmpConstStr(attr, "if") == 0) {
                        return IF;
                    } else if (strCmpConstStr(attr, "not") == 0) {
                        return NOT;
                    } else if (strCmpConstStr(attr, "nil") == 0) {
                        return NIL;
                    } else if (strCmpConstStr(attr, "then") == 0) {
                        return THEN;
                    } else if (strCmpConstStr(attr, "while") == 0) {
                        return WHILE;
                    } else if (strCmpConstStr(attr, "inputs" ) == 0) {
                        return INPUTS;
                    } else if (strCmpConstStr(attr, "inputi" ) == 0) {
                        return INPUTI;
                    } else if (strCmpConstStr(attr, "inputf" ) == 0) {
                        return INPUTF;
                    } else if (strCmpConstStr(attr, "print" ) == 0) {
                        return PRINT;
                    } else if (strCmpConstStr(attr, "length" ) == 0) {
                        return LENGTH;
                    } else if (strCmpConstStr(attr, "substr" ) == 0) {
                        return SUBSTR;
                    } else if (strCmpConstStr(attr, "ord" ) == 0) {
                        return ORD;
                    } else if (strCmpConstStr(attr, "chr" ) == 0) {
                        return CHR;
                    } else
                        return ID;
                }
                break;



            case LESS_STATE:
                if (c == '=') {
                    return LESS_OR_EQUAL;
                } else {
                    ungetc(c, stdin);
                    return LESS;
                }


            case GREATER_STATE:
                if (c == '=') {
                    return GREATER_OR_EQUAL;
                } else {
                    ungetc(c, stdin);
                    return GREATER;
                }

            case EXCLAMATION_STATE:
                if (c == '=') {
                    return NOT_EQUAL;
                } else {
                    ungetc(c, stdin);
                    return ERR_LEX;
                }

            case NUMBER_STATE:
                //som v stave NUMBER STATE, pokial nacitavam cislice
                if (isdigit(c)) {

                    //prva cislica je nula
                    if (zero_counter == 1) {
                        zero_counter = 0;
                        num_err = 1;
                    }
                    state = NUMBER_STATE;
                    strAddChar(attr, c);
                } else if (c == '.') {
                    state = DOT_STATE;
                    strAddChar(attr, c);
                } else if ((c == 'e') || (c == 'E')) {
                    state = EXP_STATE;
                    strAddChar(attr, c);

                    //pride nieco ine ako cislica, e, E alebo bodka
                } else {
                    if (num_err == 0) {
                        //prva cislica nebola nula --> vrat token NUMBER
                        ungetc(c, stdin);
                        return NUMBER;

                    //prva cislica bola nula  --> lexikalna chyba
                    } else {
                        ungetc(c, stdin);
                        return ERR_LEX;
                    }
                }
                break;

            case DOT_STATE:
                if (isdigit(c)) {
                    state = DOT_PART_STATE;
                    strAddChar(attr, c);
                } else {
                    //za bodkou prislo nieco ine ako cislica --> lexikalna chyba
                    ungetc(c, stdin);
                    return ERR_LEX;
                }
                break;

            case DOT_PART_STATE:
                if (isdigit(c)) {
                    state = DOT_PART_STATE;
                    strAddChar(attr, c);
                } else if ((c == 'e') || (c == 'E')) {
                    state = EXP_STATE;
                    strAddChar(attr, c);
                } else {
                    ungetc(c, stdin);
                    return NUMBER;
                }
                break;

            case EXP_STATE:
                if ((c == '+') || (c == '-')) {
                    state = SIGN_STATE;
                    strAddChar(attr, c);
                } else if ((isdigit(c)) && (c != '0')) {
                    state = EXP_PART_STATE;
                    strAddChar(attr, c);
                } else if (c == '0') {
                    //odstranujeme prebytocne nuly na zaciatku
                    zero_counter++;
                    state = EXP_STATE;
                } else {
                    if (zero_counter == 0) {
                        ungetc(c, stdin);
                        return ERR_LEX;
                    } else {
                        zero_counter = 0;
                        strAddChar(attr, '0');
                        ungetc(c, stdin);
                        return NUMBER;
                    }
                }
                break;

            case SIGN_STATE:
                if ((isdigit(c)) && (c != '0')) {
                    state = EXP_PART_STATE;
                    strAddChar(attr, c);
                } else if (c == '0') {
                    //odstranujeme prebytocne nuly na zaciatku
                    zero_counter++;
                    state = SIGN_STATE;
                } else {
                    if (zero_counter == 0) {
                        ungetc(c, stdin);
                        return ERR_LEX;
                    } else {
                        zero_counter = 0;
                        strAddChar(attr, '0');
                        ungetc(c, stdin);
                        return NUMBER;
                    }
                }
                break;

            case EXP_PART_STATE:
                if (isdigit(c)) {
                    state = EXP_PART_STATE;
                    strAddChar(attr, c);
                } else {
                    ungetc(c, stdin);
                    return NUMBER;
                }
                break;

            case STRING_STATE:
                if (c == '"') {
                    return STRING;
                } else if (c == '\\') {
                    state = ESCAPE_STATE;
                } else if ((int) c > 31) {
                    state = STRING_STATE;
                    strAddChar(attr, c);
                } else {
                    // Prijde jiny znak nez hexa, zapsany escape sekvenci, nebo s hodnotou ascii > 31
                    return ERR_LEX;
                }
                break;

            case ESCAPE_STATE:
                if (c == 'n') {
                    state = STRING_STATE;
                    c = '\n';
                    strAddChar(attr, c);

                } else if (c == 't') {
                    state = STRING_STATE;
                    c = '\t';
                    strAddChar(attr, c);

                } else if (c == 's') {
                    state = STRING_STATE;
                    c = ' ';
                    strAddChar(attr, c);

                } else if (c == '\\') {
                    state = STRING_STATE;
                    strAddChar(attr, c);

                } else if (c == '"') {
                    state = STRING_STATE;
                    strAddChar(attr, c);

                } else if (c == 'x') {
                    state = HEX_STATE;

                } else if ((int) c > 31) {
                    state = STRING_STATE;
                    strAddChar(attr, c);

                } else {
                    return ERR_LEX;
                }
                break;

            case HEX_STATE:
                if (isdigit(c) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'))) {
                    state = HEX_NUMBER_STATE;
                    hexa[0] = c;
                } else {
                    return ERR_LEX;
                }
                break;

            case HEX_NUMBER_STATE:
                // \ escape sekvence - hexa cislo se prevede a jde se do stavu ESCAPE_STATE
                if (c == '\\') {
                    state = ESCAPE_STATE;
                    hexa[1] = '\0';
                    int num;
                    hex_to_int(hexa, &num);
                    strAddChar(attr, num);

                } else if (isdigit(c) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'))) {
                    // prijeti 0-F -> prevod a prechod do STRING_STATE
                    state = STRING_STATE;
                    hexa[1] = c;
                    hexa[2] = '\0';
                    int num;
                    hex_to_int(hexa, &num);
                    strAddChar(attr, num);


                } else if (c == '"') {
                    // prijeti uvozovek - konec stringu - return STRING
                    return STRING;

                } else if (c == '\n') {
                    // novy radek -> neukonceny retezec -> ERR_LEX
                    return ERR_LEX;

                } else {
                    // ostatni -> prechod do STRING STATE
                    state = STRING_STATE;
                    hexa[1] = '\0';
                    int num;
                    hex_to_int(hexa, &num);
                    strAddChar(attr, num);
                    ungetc(c, stdin);


                }
                break;

            case LINE_COMM_STATE:
                if (c == '\n') {
                    //novy riadok - prechod do INIT STATE
                    state = INIT_STATE;
                } else {
                    state = LINE_COMM_STATE;
                }
                break;

            case EQUAL_STATE:
                if (new_line == 1) {
                    //znak '=' je na novom riadku
                    do {
                        if (c == EOF) {
                            //ak pride EOF vrat dopredu nacitane znaky a token priradenie
                            char *to_ret = strGetStr(begin_str);
                            for (int i = strGetLength(begin_str) - 1; i >= 0; i--) {
                                ungetc(to_ret[i], stdin);
                                new_line = 0;
                                return ASSIGN;
                            }
                        }

                        //pridaj znak do pomocneho retazca a nacitaj dalsi
                        strAddChar(begin_str, c);
                        c = getchar();

                    } while (strGetLength(begin_str) < 5);

                    //ak pride EOF vrat dopredu nacitane znaky a token priradenie
                    if (c == EOF) {
                        char *to_ret = strGetStr(begin_str);
                        for (int i = strGetLength(begin_str) - 1; i >= 0; i--) {
                            ungetc(to_ret[i], stdin);
                            new_line = 0;
                            return ASSIGN;
                        }
                    }

                    //pojde o blokovy komentar
                    if ((isspace(c) || (c == '\n')) && ((strCmpConstStr(begin_str, "begin") == 0))) {
                        new_line = 0;
                        state = BLOCK_COMM_STATE;
                        strClear(begin_str);
                        ungetc(c, stdin);
                        char *to_ret = strGetStr(begin_str);
                        for (int i = strGetLength(begin_str) - 1; i >= 0; i--) {
                            ungetc(to_ret[i], stdin);
                        }
                        new_line = 0;
                        return ASSIGN;
                    }

                    //nie '=' nie je na novom riadku
                } else {
                    if (c == '=') {
                        return COMPARE;
                    } else {
                        ungetc(c, stdin);
                        return ASSIGN;
                    }
                }
                break;

            case NEW_LINE_STATE:
                if (c == '=') {
                    new_line = 1;
                    state = EQUAL_STATE;

                } else {
                    state = INIT_STATE;
                    ungetc(c, stdin);
                    //     printf("Nasel se EOL s hodnotou: %d", c);
                    return EOL;
                    //  printf("NOVY RADEK NEZACINA ROVNA SE, JDE SE DO INITU\n");
                }
                break;

            case BLOCK_COMM_STATE:
                if (c == '\n') {
                    // Nacteme 4 dalsi znaky
                    do {
                        c = getchar();
                        strAddChar(begin_str, c);
                        if (c == EOF) {
                            return ERR_LEX;
                        }
                    } while (strGetLength(begin_str) < 4);

                    //nacitame dalsi znak
                    c = getchar();

                    //ak je nacitany znak noveho riadku a v pomocnom stringu je 'end' --> koniec blok. komentara
                    if ((c == '\n') && (strCmpConstStr(begin_str, "=end") == 0)) {
                        strClear(begin_str);
                        state = INIT_STATE;

                    //ak je nacitany iny biely znak a v pomocnom stringu je 'end' --> BLOCK COMM END STATE
                    } else if (isspace(c) && ((strCmpConstStr(begin_str, "=end") == 0))) {
                        state = BLOCK_COMM_END_STATE;
                        strClear(begin_str);

                    //pokracujeme v komentari
                    } else {
                        strClear(begin_str);
                    }
                } else {
                    state = BLOCK_COMM_STATE;
                }
                break;

            case BLOCK_COMM_END_STATE:
                if (c == '\n') {
                    state = INIT_STATE;
                    return EOL;
                } else {
                    state = BLOCK_COMM_END_STATE;
                }
        }
    }

}
