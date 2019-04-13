/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */

#include <stdlib.h>
#include <string.h>
#include "expressions.h"
#include "scanner.h"
#include "stack.h"
#include "errors.h"
#include "parser.h"

#define NUM_OF_RULES    5
#define NO_OP           100

char precedence_table [15][15] =
                            // *   /   +   -   <   >  <=  >=  ==  !=   (   )  id   $   =
                            //------------------------------------------------------------
                /* 	   *|*/ {{'>','>','>','>','>','>','>','>','>','>','<','>','<','>','X'},
                /* 	   /|*/	 {'>','>','>','>','>','>','>','>','>','>','<','>','<','>','X'},
                /* 	   +|*/	 {'<','<','>','>','>','>','>','>','>','>','<','>','<','>','X'},
                /* 	   -|*/	 {'<','<','>','>','>','>','>','>','>','>','<','>','<','>','X'},
                /* 	   <|*/	 {'<','<','<','<','>','>','>','>','>','>','<','>','<','>','X'},
                /* 	   >|*/	 {'<','<','<','<','>','>','>','>','>','>','<','>','<','>','X'},
                /*	  <=|*/  {'<','<','<','<','>','>','>','>','>','>','<','>','<','>','X'},
                /*	  >=|*/	 {'<','<','<','<','>','>','>','>','>','>','<','>','<','>','X'},
                /*	  ==|*/	 {'<','<','<','<','<','<','<','<','>','>','<','>','<','>','X'},
                /*	  !=|*/	 {'<','<','<','<','<','<','<','<','>','>','<','>','<','>','X'},
                /* 	   (|*/	 {'<','<','<','<','<','<','<','<','<','<','<','=','<','X','X'},
                /* 	   )|*/	 {'>','>','>','>','>','>','>','>','>','>','X','>','X','>','X'},
                /*	  id|*/	 {'>','>','>','>','>','>','>','>','>','>','X','>','X','>','='},
                /* 	   $|*/	 {'<','<','<','<','<','<','<','<','<','<','<','X','<','X','X'},
                /* 	   =|*/	 {'<','<','<','<','<','<','<','<','<','<','<','>','X','>','X'}};


// Mapovaci funkce
int getIndex(int i)
{
    switch(i)
    {
        case OPER_MUL:
            return MUL_PREC;
        case OPER_DIV:
            return DIV_PREC;
        case OPER_PLUS:
            return ADD_PREC;
        case OPER_MINUS:
            return SUB_PREC;
        case LESS:
            return LESS_PREC;
        case GREATER:
            return GREAT_PREC;
        case LESS_OR_EQUAL:
            return LESS_EQL_PREC;
        case GREATER_OR_EQUAL:
            return GREAT_EQL_PREC;
        case COMPARE:
            return EQL_PREC;
        case NOT_EQUAL:
            return NOT_EQL_PREC;
        case LEFT_BRACKET:
            return LEFT_BRACKET_PREC;
        case RIGHT_BRACKET:
            return RIGHT_BRACKET_PREC;
        case 'i' :
            return ID_PREC;
        case NUMBER:
            return ID_PREC;
        case STRING:
            return ID_PREC;
        case ID:
            return ID_PREC;
        case ASSIGN:
            return ASSIGN_PREC;
        case 1:
            return DOLLAR_PREC;
    }

    return 99;
}

void string_concat(string *result, string *to_concate) {

    for(int i = 0; i < strGetLength(to_concate); i++) {
        strAddChar(result, to_concate->str[i]);
    }
}

int reduce(struct Stack* stack) {

    char rules[][NUM_OF_RULES] = {"i", "E+E","E-E", "E*E", "E/E"};
    int rule_len;

    int found = 0;

    // Vyber pravidla
    for (int i = 0; i < NUM_OF_RULES; i++) {

        //("Testujeme pravidlo: %s\n", rules[i]);

        rule_len = strlen(rules[i]);

        if (stack->top < rule_len) {
            continue;
        }

        char type;
        // Projdeme pocet polozek ze stacku, jaka je delka pravidla
        for (int j = 0; j < rule_len; j++) {
            type = stack->array[stack->top-j].type;
            //("KOUKAME SE NA: %d\n", type);

            //("Hodnota j je: %d\n", j);

            // Porovnavame polozku na stacku se znakem pravidla
            if (rules[i][j] == type) {
                ////("Udajne je stejny tohle %c a tohle %c!!!!\n",rules[i][j], type);
                ////("Hodnota j je: %d\n", j);
                found = 1;
            } else if (rules[i][j] == '+' && type == OPER_PLUS) {
                found = 1;
            } else if (rules[i][j] == '-' && type == OPER_MINUS) {
                found = 1;
            } else if (rules[i][j] == '*' && type == OPER_MUL) {
                found = 1;
            } else if (rules[i][j] == '/' && type == OPER_DIV) {
                found = 1;
            } else {

                found = 0;
                break;
                //continue;
            }

        }
        if (found == 1) {
            // Probehne redukce
            if (type == 'i') {
                //("REDUKCE E->i!\n");
                stack->array[stack->top].type = 'E';
                return 1;
            } else {
                if (stack->array[stack->top-1].type == OPER_MUL) {
                    //("REDUKCE E->E*E\n");
                    for(int k = 0; k < rule_len; k++) {
                        pop(stack);
                    }

                    struct Stack_elem st_elem;
                    st_elem.type = 'E';
                    push(stack, st_elem);

                    /* random int between 0 and 19 */
                    int r = rand() % 999999999999;
                    Generate_stack_muls(r);

                    return 1;
                } else if (stack->array[stack->top-1].type == OPER_DIV) {
                    //("REDUKCE E->E/E\n");
                    for(int k = 0; k < rule_len; k++) {
                        pop(stack);
                    }

                    struct Stack_elem st_elem;
                    st_elem.type = 'E';
                    push(stack, st_elem);

                    /* random int between 0 and 19 */
                    int r = rand() % 999999999999;
                    Generate_stack_dividing(r);

                    return 1;
                } else if (stack->array[stack->top-1].type == OPER_PLUS) {
                    //("REDUKCE E->E+E\n")
                    for(int k = 0; k < rule_len; k++) {
                        pop(stack);
                    }

                    struct Stack_elem st_elem;
                  //  st_elem.value = val;
                    st_elem.type = 'E';
                    push(stack, st_elem);

                    /* random int between 0 and 19 */
                    int r = rand() % 999999999999;
                    Generate_stack_adds(r);

                    return 1;
                } else if (stack->array[stack->top-1].type == OPER_MINUS) {
                    //("REDUKCE E->E-E\n");
                    for(int k = 0; k < rule_len; k++) {
                        pop(stack);
                    }

                    struct Stack_elem st_elem;
                    st_elem.type = 'E';
                    push(stack, st_elem);


                    /* random int between 0 and 19 */
                    int r = rand() % 999999999999;
                    Generate_stack_subs(r);

                    return 1;
                }
            }
        }
    }
    //("Pravidlo nenalezeno, ukoncuje se redukce\n");
    return 0;
}


int get_expression(char* firstID, int token, string attr1, char *variabile) {

    if (token == EOL) {
        Generate_push_LF(firstID);
        Generate_final_move_int_var(variabile);
        return SYN_OK;
    }

    int return_code;
    // pomocne strince pro getNextToken
    string begin_str;
    strInit(&begin_str);
    // string, ve kterem dostaneme hodnotu tokenu
    string attr = attr;
    strInit(&attr);

    // Vytvoreni stacku pro 50 polozek
    struct Stack *stack = createStack(50);

    // Vytvoreni prvni zarazky, kterou pridame do stacku
    struct Stack_elem first_st_elem;
    first_st_elem.type = DOLLAR_PREC;
    push(stack, first_st_elem);

    // Vytvoreni prvniho prvku, ktery pridame do stacku
    struct Stack_elem st_elem;
    st_elem.type = 'i';
    if (firstID != NULL) {
        st_elem.const_or_var = 1;
        st_elem.var_name = firstID;
     //   printf("PUSHLO SE ID S NAZVEM: %s\n", firstID);
        Generate_push_LF(firstID);
    } else {
        st_elem.const_or_var = 0;
        st_elem.const_val = attr1.str;
     //   printf("PUSHLO SE CISLO S HODNOTOU: %s\n", attr.str);
        if (token == NUMBER) {
            Generate_push_number(attr1.str);
        }else if (token == STRING){
            Generate_push_string(attr1.str);
        }
    }

    push(stack, st_elem);


    char prec_symbol;


    if (firstID != NULL) {
        if (token == ERR_LEX) {
            strFree(&attr);
            strFree(&begin_str);

            return ERR_LEX;
        }

   //     printf("#TYP NA VRCHOLU STACKU JE: %d\n", top(stack)->type);
   //     printf("#TOKEN JE: %d\n", token);

        int x = getIndex((int)top(stack)->type);
        int y = getIndex(token);

        if(x == 99 || y == 99){
            //(stderr, "Doslo k chybe!!!");
            strFree(&attr);
            strFree(&begin_str);
            disposeStack(stack);
            return ERR_SYN;
        }

        prec_symbol = precedence_table[x][y];

        if (prec_symbol == '<') {
            //("Ma mensi prioritu, bude push\n");

            struct Stack_elem st_elem;

            if (token == NUMBER || token == STRING || token == ID) {
                st_elem.type = 'i';
            }
            push(stack, st_elem);
        }
        else if (prec_symbol == '>') {
            int reduce_retval;

//printf("%s\n",attr.str);
            while((reduce_retval = reduce(stack))) {
                if (reduce_retval == ERR_ZERO_DIVISION) {
                    strFree(&attr);
                    strFree(&begin_str);
                    disposeStack(stack);
                    return ERR_ZERO_DIVISION;
                }
                if (top(stack)->type == 'E') {
                    prec_symbol = precedence_table[getIndex((int)stack->array[stack->top-1].type)][getIndex(token)];
                } else {
                    prec_symbol = precedence_table[getIndex((int)top(stack)->type)][getIndex(token)];
                }

                if (prec_symbol != '>')
                    break;
            };

            struct Stack_elem st_elem;

            if (token == NUMBER) {
                //("Co mam delat :(\n");
            } else if (token == OPER_PLUS) {
                st_elem.type = OPER_PLUS;
                //("Do stacku se pushlo +\n");
            } else if (token == OPER_MINUS) {
                st_elem.type = OPER_MINUS;
                //("Do stacku se pushlo -\n");
            } else if (token == OPER_MUL) {
                st_elem.type = OPER_MUL;
                //("Do stacku se pushlo *\n");
            } else if (token == OPER_DIV) {
                st_elem.type = OPER_DIV;
                //("To stacku se pushlo /\n");
            }
            push(stack, st_elem);

        } else if (prec_symbol == 'X') {
            //(stderr, "NASTALA CHYBA PRECEDENCNI ANALYZY!\n");
      //      strFree(&attr);
            strFree(&begin_str);
            disposeStack(stack);
            return ERR_SYN;
        }

    }


    while(((token = getNextToken(&attr, &begin_str)) != EOL)) {

        if (token == ERR_LEX) {
            strFree(&attr);
            strFree(&begin_str);

            return ERR_LEX;
        }



        //("NACETL SE TOKEN S HODNOTOU: %d\n", token);

        if (token == ID) {
            //("NASEL SE IDENTIFIKATOR SE HODNOTOU: %s\n", attr.str);

            if(isDeclared(attr.str)){
                //("JE DEKLAROVANE\n");
            } else {
                //("NENI DEKLAROVANE\n");
                strFree(&begin_str);
                return ERR_SEM;
            }
        }

         //("TOKEN: %d\n",token );


        if (top(stack)->type == 'E') {
            int x = getIndex((int)stack->array[stack->top-1].type);
            int y = getIndex(token);
            if(x == 99 || y == 99){
                //(stderr, "Doslo k chybe!!!");
                strFree(&attr);
                strFree(&begin_str);
                disposeStack(stack);
                return ERR_SYN;
            }
            prec_symbol = precedence_table[x][y];
        }
        else {
            int x = getIndex((int)top(stack)->type);
            int y = getIndex(token);

            if(x == 99 || y == 99){
                //(stderr, "Doslo k chybe!!!");
                strFree(&attr);
                strFree(&begin_str);
                disposeStack(stack);
                return ERR_SYN;
            }

            prec_symbol = precedence_table[x][y];

            //("INDEXY DO PRECEDENCNI TABULKY JSOU: %d a %d : \n", getIndex((int)top(stack)->type),getIndex(token));
            //("Dostali jsme z tabulky znak: %c\n", prec_symbol);
        }



        if (prec_symbol == '<') {
            //("Ma mensi prioritu, bude push\n");

            struct Stack_elem st_elem;

            if (token == NUMBER || token == STRING || token == ID) {
                st_elem.type = 'i';
                if (token == ID) {
                    st_elem.const_or_var = 1;
                    st_elem.var_name = firstID;
              //      printf("PUSHLO SE ID S NAZVEM: %s\n", firstID);
                    Generate_push_LF(attr.str);
                } else {
                    st_elem.const_or_var = 0;
                    st_elem.const_val = attr.str;

                    if (token == NUMBER) {
                        Generate_push_number(attr.str);
                    } else if (token == STRING){
                        Generate_push_string(attr.str);
                    }
                }
            }
            push(stack, st_elem);

        } else if (prec_symbol == '>') {
            int reduce_retval;
      //      printf("%s\n",attr.str);

            while((reduce_retval = reduce(stack))) {
                if (reduce_retval == ERR_ZERO_DIVISION) {
                    strFree(&begin_str);
                    disposeStack(stack);
                    return ERR_ZERO_DIVISION;
                }
                if (top(stack)->type == 'E') {
                    prec_symbol = precedence_table[getIndex((int)stack->array[stack->top-1].type)][getIndex(token)];
                } else {
                    prec_symbol = precedence_table[getIndex((int)top(stack)->type)][getIndex(token)];
                }

                if (prec_symbol != '>')
                    break;
            };

            struct Stack_elem st_elem;

            if (token == NUMBER) {
                //("Co mam delat :(\n");
            } else if (token == OPER_PLUS) {
                st_elem.type = OPER_PLUS;
                //("Do stacku se pushlo +\n");
            } else if (token == OPER_MINUS) {
                st_elem.type = OPER_MINUS;
                //("Do stacku se pushlo -\n");
            } else if (token == OPER_MUL) {
                st_elem.type = OPER_MUL;
                //("Do stacku se pushlo *\n");
            } else if (token == OPER_DIV) {
                st_elem.type = OPER_DIV;
                //("To stacku se pushlo /\n");
            }
            push(stack, st_elem);

        } else if (prec_symbol == 'X') {
            //(stderr, "NASTALA CHYBA PRECEDENCNI ANALYZY!\n");
            strFree(&attr);
            strFree(&begin_str);
            disposeStack(stack);
            return ERR_SYN;
        }

    }

    //("JSME NA KONCI VYRAZU, JDEME TO REDUKOVAT!\n");
    while (reduce(stack));

    // Redukce ciselneho vyrazu probehla v poradku a ve stacku nam zbylo jedine E
    if (stack->top+1 == 2) {
        //("Redukce ciselneho vyrazu probehla v poradku!\n");
        // TODO MOVE VYSLEDEK NA VRCHOLU STACKU DO VARIABILE
        Generate_final_move_int_var(variabile);
        return_code = SYN_OK;
    }
    // Redukce neprobehla v poradku, z nejakych duvodu se nepovedlo redukovat
    else {
        //(stderr,"Pri redukci doslo z nejakeho duvodu k chybe!\n");
        return_code = ERR_SYN;
    }


    strFree(&attr);
    strFree(&begin_str);
    disposeStack(stack);

    return return_code;
}

int get_if_or_while_expression(int then_or_do, int token, string attr, int random) {
    int return_code;

    // pomocne strince pro getNextToken
    string begin_str;
    strInit(&begin_str);

    // Vytvoreni stacku pro 50 polozek
    struct Stack *stack = createStack(50);

    // Vytvoreni prvni zarazky, kterou pridame do stacku
    struct Stack_elem first_st_elem;
    first_st_elem.type = DOLLAR_PREC;
    push(stack, first_st_elem);

    // Vytvoreni prvniho prvku, ktery pridame do stacku
    struct Stack_elem st_elem;
    st_elem.type = 'i';

    if (token == NUMBER) {
        Generate_push_number(attr.str);
    } else if (token == STRING){
        Generate_push_string(attr.str);
    } else if (token == ID) {
        Generate_push_LF(attr.str);
    }


    push(stack, st_elem);


    char prec_symbol;


    while(((token = getNextToken(&attr, &begin_str)) != then_or_do)) {

        if (token == ERR_LEX) {
           // strFree(&attr);
            strFree(&begin_str);

            return ERR_LEX;
        }


        if (token == ID) {

            if(isDeclared(attr.str)){
                //("JE DEKLAROVANE\n");
            } else {
                //("NENI DEKLAROVANE\n");
                strFree(&begin_str);
                return ERR_SEM;
            }
        }

        if (top(stack)->type == 'E') {
            int x = getIndex((int)stack->array[stack->top-1].type);
            int y = getIndex(token);
            if(x == 99 || y == 99){
                strFree(&begin_str);
                disposeStack(stack);
                return ERR_SYN;
            }
            prec_symbol = precedence_table[x][y];
        }
        else {
            int x = getIndex((int)top(stack)->type);
            int y = getIndex(token);

            if(x == 99 || y == 99){
                strFree(&begin_str);
                disposeStack(stack);
                return ERR_SYN;
            }

            prec_symbol = precedence_table[x][y];

        }



        if (prec_symbol == '<') {
            //("Ma mensi prioritu, bude push\n");

            struct Stack_elem st_elem;

            if (token == NUMBER || token == STRING || token == ID) {
                st_elem.type = 'i';
                if (token == ID) {
                    //      printf("PUSHLO SE ID S NAZVEM: %s\n", firstID);
                    Generate_push_LF(attr.str);
                } else {
                    if (token == NUMBER) {
                        Generate_push_number(attr.str);
                    } else if (token == STRING){
                        Generate_push_string(attr.str);
                    }
                }
            }
            push(stack, st_elem);

        } else if (prec_symbol == '>') {
            int reduce_retval;

            while((reduce_retval = reduce(stack))) {
                if (reduce_retval == ERR_ZERO_DIVISION) {
                   // strFree(&attr);
                    strFree(&begin_str);
                    disposeStack(stack);
                    return ERR_ZERO_DIVISION;
                }
                if (top(stack)->type == 'E') {
                    prec_symbol = precedence_table[getIndex((int)stack->array[stack->top-1].type)][getIndex(token)];
                } else {
                    prec_symbol = precedence_table[getIndex((int)top(stack)->type)][getIndex(token)];
                }

                if (prec_symbol != '>')
                    break;
            };

            struct Stack_elem st_elem;

            if (token == NUMBER) {
                //("Co mam delat :( \n");
            } else if (token == OPER_PLUS) {
                st_elem.type = OPER_PLUS;
                //("Do stacku se pushlo +\n");
            } else if (token == OPER_MINUS) {
                st_elem.type = OPER_MINUS;
                //("Do stacku se pushlo -\n");
            } else if (token == OPER_MUL) {
                st_elem.type = OPER_MUL;
                //("Do stacku se pushlo *\n");
            } else if (token == OPER_DIV) {
                st_elem.type = OPER_DIV;
                //("Do stacku se pushlo /\n");
            } else if (token == LESS) {
                st_elem.type = LESS;
                //("Do stacku se pushlo <\n");
            } else if (token == LESS_OR_EQUAL) {
                st_elem.type = LESS_OR_EQUAL;
                //("Do stacku se pushlo <=\n");
            } else if (token == COMPARE) {
                st_elem.type = COMPARE;
                //("Do stacku se pushlo == \n");
            } else if (token == GREATER) {
                st_elem.type = GREATER;
                //("Do stacku se pushlo > \n");
            } else if (token == GREATER_OR_EQUAL) {
                st_elem.type = GREATER_OR_EQUAL;
                //("Do stacku se pushlo >=\n");
            } else if (token == NOT_EQUAL) {
                st_elem.type = NOT_EQUAL;
                //("Do stacku se pushlo !=\n");
            }
            push(stack, st_elem);

        } else if (prec_symbol == 'X') {
            //(stderr, "NASTALA CHYBA PRECEDENCNI ANALYZY!\n");
           // strFree(&attr);
            strFree(&begin_str);
            disposeStack(stack);
            return ERR_SYN;
        }

    }

    //("JSME NA KONCI VYRAZU, JDEME TO REDUKOVAT!\n");

    while (reduce(stack));


    // Redukce vyrazu probehla v poradku a ve stacku nam zbylo: E <relacni operator> E
     if (stack->top+1 == 4) {
        //("Vyhodnoceni IF OR WHILE probehlo v poradku.\n");
        return_code = SYN_OK;

        int rel_op = stack->array[stack->top-1].type;

        switch (rel_op) {

            case LESS:
            case GREATER:
                printf("\n\n#GREAT OR LESS\n\n");
                Generate_less_great(random, rel_op);
                break;

            case LESS_OR_EQUAL:
            case GREATER_OR_EQUAL:
                printf("\n\n#LESS/GREATER OR EQUAL\n\n");
                Generate_less_great_eq(random, rel_op);
                break;

            case COMPARE:
            case NOT_EQUAL:
                printf("\n\n#COMPARISON\n\n");
                Generate_eq(random, rel_op);
                break;
        }



    } else {
        //(stderr, "Doslo k chybe!\n");
        return_code = ERR_SYN;
    }

    //strFree(&attr);
    strFree(&begin_str);
    disposeStack(stack);

    return return_code;
}
