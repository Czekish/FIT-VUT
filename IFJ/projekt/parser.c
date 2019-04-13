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
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "parser.h"
#include "errors.h"

#define FUNC_TABLE 0
#define LOCAL_V_TABLE 1

// Makro ktore vracia index do tabulky symbolov
#define GET_INDEX(X)  ((X) == DEF ? 0 : 1)

string attr; // globani promena, ulozen atribut tokenu
string begin_str; // pomocny string po rozpoznani komentare
string pomStr;
string funcName; // pomocny string pre ulozenie nazvu funkcie pri definicii funkcie
string locVariabile; // pomocny string pri priradovani do premennej => a = 5 (locVariabile == a)
tBSTNodePtr symtable[2]; // 2 tabulky symbolov -> 1.pre funckie a 2. pre premenne z hlavneho tela programu
int indexToTS; // index do Tabulky symbolov
int token; // Globalna premenna do ktorej sa uklada nacitany token
bool isLeft = FALSE;
char **passVariabile = NULL;
bool readParams = false;

bool isDeclared(char *locVariabile){

  if(indexToTS == FUNC_TABLE){
    tBSTNodePtr node = BSTSearch(symtable[FUNC_TABLE],funcName.str);
    if(node == NULL) // Nemalo by sa stat
      return false;
    tBSTNodePtr nodeinTSF = BSTSearch(node->TS,locVariabile);
    if(nodeinTSF == NULL){
      BSTInsert(&node->TS,locVariabile,TRUE,0,"nil");
      return FALSE;
    }
    else{
      if(nodeinTSF->Declared)
        return TRUE;
      else{
        nodeinTSF->Declared = true;
        return false;
      }
    }
  }
  else{
    tBSTNodePtr node = BSTSearch(symtable[LOCAL_V_TABLE],locVariabile);
    if(node == NULL){
      BSTInsert(&symtable[LOCAL_V_TABLE],locVariabile,FALSE,0,"nil");
    }
    else{
      if(node->Declared){
        return TRUE;
      }
      else{
        node->Declared = true;
        return FALSE;
      }
    }


    if(!(BSTSearch(symtable[LOCAL_V_TABLE],locVariabile)->Declared)){
      BSTSearch(symtable[LOCAL_V_TABLE],locVariabile)->Declared = TRUE;
      return FALSE;
    }
    else
      return true;
  }
}

int expression_precedence(int token, char *variabile, char *firstID) {

    int ret_code;

    if (firstID != NULL) {
        // V TOKENU JE TOKEN NASLEDUJICI PO IDENTIFIKATORU
        if(isDeclared(firstID)){
           // printf("JE DEKLAROVANE\n");
        } else {
           // printf("NENI DEKLAROVANE\n");
            return ERR_SEM;
        }
        ret_code = get_expression(firstID, token, attr, variabile);
    } else if (token == STRING || token == NUMBER || token == EOL) {
        ret_code = get_expression(NULL, token, attr, variabile);
    }

    return ret_code;
}


int if_or_while_precedence(int then_or_do, int token, int random) {
    int ret_code;

    if (token == STRING || token == NUMBER) {
        ret_code = get_if_or_while_expression(then_or_do, token, attr, random);
    } else if (token == ID) {
        if(isDeclared(attr.str)) {
          // je declarovane, ok
        } else {
          // neni deklarovane
          return ERR_SEM;
        }
        ret_code = get_if_or_while_expression(then_or_do, token, attr, random);
    }

    return ret_code;
}

int freeAll(){

  int err = SYN_OK;
  strFree(&attr);
  strFree(&begin_str);
  strFree(&pomStr);
  strFree(&funcName);
  strFree(&locVariabile);
  BSTDispose(&symtable[1],&err);
  BSTDispose(&symtable[0],&err); // Ci na konci programu existuju nejake nedeklarovane funkcie
  return err;
}

int statList(){

  int err = SYN_OK;

  strInit(&attr);
  strInit(&begin_str);
  strInit(&pomStr);
  strInit(&funcName); // Id funkcie ked budeme chciet ukladat lokalne premenne funckie do TS funckie
  strInit(&locVariabile); // Id lokalnej premennej funkcie pre pripadne buduce ukladanie do TS
  strcpy(locVariabile.str,"NULL");
  BSTInit(&symtable[0]);
  BSTInit(&symtable[1]);
  Generate_start_file();

  if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
    return ERR_LEX;

  while(token != END_OF_FILE && err == SYN_OK){ // po koniec zdrojaku alebo ak sa najde chyba


      if(token != EOL){ // preskaceme orazdne riadky
        indexToTS = GET_INDEX(token);
        err = stat(); // <stat><st-list>
      }

      if(err != SYN_OK){
        freeAll();
        return err;
      }

      if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
        return ERR_LEX;
  }

  printf("POPFRAME\n");
  return freeAll();

}

int stat(){

  int random = rand() % 999999999999;
  int err = SYN_OK;

  switch (token) {

    case DEF: // def id (<item-list>) EOL <st-list> end EOL

      if(getNextToken(&attr,&begin_str) == ID && (token = getNextToken(&pomStr,&begin_str)) == LEFT_BRACKET){

          strCopyString(&funcName, &attr);
          int NumberOfParams = 0;
          readParams = true;
          err = itemList(&NumberOfParams,locVariabile.str); // spracovanie vnutra zatvorky
          isLeft = false;
          Generate_func_start(funcName.str,NumberOfParams,passVariabile);
          readParams = false;

          // pridavam do TBS, ak tam este nie je inak vraciame semanticku chybu
          if(!BSTSearch(symtable[indexToTS],funcName.str))
              BSTInsert (&symtable[indexToTS],funcName.str,TRUE,NumberOfParams,NULL);
          else // pokus o redefiniciu
            return ERR_SEM;


          if(passVariabile != NULL){
            for (size_t i = 0; i<NumberOfParams ;i++) {

                BSTInsert(&(BSTSearch(symtable[indexToTS],funcName.str)->TS),passVariabile[i],TRUE,0,NULL);
                free(passVariabile[i]);
              }
              free(passVariabile);
          }

          if(err != SYN_OK)
            return err;
          else if(getNextToken(&attr,&begin_str) != EOL)
            return ERR_SYN;


        // spracovanie tela funkcie
        while((token = getNextToken(&attr,&begin_str)) != END){

          if(token == EOL) // preskaceme priazdne riadky
            continue;

          if(token == END_OF_FILE) // koniec zdrojaku pred klucovym slovom END || chyba lexemu
            return ERR_SYN;
          else if(token == ERR_LEX)
            return ERR_LEX;

          err = stat();

          if(err != SYN_OK)
            return err;
        }

        // TODO GENERATE FUNCTION END
        Generate_func_end(funcName.str);
        return err; // koniec tela funkcie
      }
      else{
        return ERR_SYN;
      }

      break; //  END OF def id (<item-list>) EOL <st-list> end EOL


    case IF: // if <expr> then EOL <st-list> else <st-list> end EOL

      if((token = getNextToken(&attr, &begin_str)) == ERR_LEX)
        return ERR_LEX;


        printf("#PODMINKA IFU!!\n\n");
        err = if_or_while_precedence(THEN,token, random); // za if sa ocakava <expr>

      if(err != SYN_OK)
        return err;

      // TODO GENERATE IF expr THEN

      while((token = getNextToken(&attr,&begin_str)) != ELSE){ // spracovanie casti then

          if(token == EOL) // preskocime prazdne riadky
            continue;

          if(token == END_OF_FILE) // po then nenasleduje else -> citame do konca suboru co je chyba
            return ERR_SYN;
          else if(token == ERR_LEX)
            return ERR_LEX;

          err = stat();
          if(err != SYN_OK)
            return err;
      }

      token = getNextToken(&attr,&begin_str);
      if(token != EOL) // po else musi nasledovat EOL
        return ERR_SYN;
      else if(token == ERR_LEX)
        return ERR_LEX;

      // TODO GENERATE ELSE


      printf("JUMP $end%d\n", random);
      printf("LABEL $else%d\n", random);
      printf("#VETEV ELSE!!!\n\n");

      while((token = getNextToken(&attr,&begin_str)) != END){ // spracovanie casti else

        if(token == EOL) // preskocime prazdne riadky
          continue;

        if(token == END_OF_FILE) // po else nenasleduje end -> citame do konca suboru co je chyba
          return ERR_SYN;
        else if(token == ERR_LEX)
          return ERR_LEX;


        err = stat();
        if(err != SYN_OK)
          return err;
      }

      printf("LABEL $end%d\n", random);
      printf("#KONEC VETVE ELSE!!\n\n");

      return SYN_OK; // analyza if-u prebehla uspesne

      break; // if <expr> then EOL <st-list> else <st-list> end EOL


    case WHILE: // while <expr> do EOL <st-list> end EOL

      if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
        return ERR_LEX;

      err = if_or_while_precedence(DO,token, random); // za if sa ocakava <expr>

      if(err != SYN_OK) // po do sa ocakava EOL
        return err;

      // TODO GENERATE WHILE expr DO

      while((token = getNextToken(&attr,&begin_str)) != END){

        if(token == EOL)
          continue;

        if(token == END_OF_FILE) // na konci while nie je end -> citame do konca asuboru co je chyba
          return ERR_SYN;
        else if(token == ERR_LEX)
          return ERR_LEX;

        err = stat();
        if(err != SYN_OK)
          return err;

      }

      return SYN_OK;

      break; // END OF while <expr> do EOL <st-list> end EOL

    case ID: ;// na zaciatku je ID

      strCopyString(&pomStr, &attr);
      if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
        return ERR_LEX;

      switch(token){

        case ASSIGN: // priradenie

          if((BSTSearch(symtable[FUNC_TABLE],pomStr.str)) != NULL)
            return ERR_SEM;

          if(indexToTS == LOCAL_V_TABLE){ // Premenna z hlavneho tela programu

              if(!BSTSearch(symtable[LOCAL_V_TABLE],pomStr.str)){ // hladame premennu v TBS ak sa nenajde tak sa do nej prida
                BSTInsert(&symtable[LOCAL_V_TABLE],pomStr.str,NULL,0,"nil");
              }
          }

          strCopyString(&locVariabile,&pomStr);

          if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
            return ERR_LEX;

          err = stat();
          if(err != SYN_OK){
            return err;
          }
            return err;
          break;


        default: ;// nepriradenie

          int NumberOfParams = 0; // pocet parametrov funkcie
          tBSTNodePtr funcTable = NULL; // struktura funckie z TBS

          if(indexToTS == FUNC_TABLE){
            if((funcTable = BSTSearch(symtable[indexToTS],funcName.str)) != NULL ){// Ukladanie lokalnej premennej funkcie do TS
              if(BSTSearch(funcTable->TS,locVariabile.str) == NULL)
                BSTInsert(&funcTable->TS,locVariabile.str,FALSE,0,"nil");
            }
            else // struktura funckie sa nenasla aj ked uz mala byt inicializovana => chyba
              return ERR_INTERN; // chyba
          }

          if(token > 51 && token < 61){
            if(!(isDeclared(locVariabile.str)))
              Generate_assign_var(locVariabile.str);

            err = expression_precedence(token,locVariabile.str,pomStr.str);
          }
          // za ID, ktere je za =, nasleduje dalsi id -> jde o funkci a jeji parametry
          else if(token == ID || (token > 63 && token < 66) || token == LEFT_BRACKET){

            if(!isDeclared(locVariabile.str)){
              Generate_assign_func_var(locVariabile.str);
            }
            else{
              printf("CREATEFRAME\n");
            }
            err = itemList(&NumberOfParams,locVariabile.str);
            isLeft = false;
            printf("CALL $%s\n",pomStr.str);
            Generate_to_func_var(locVariabile.str,indexToTS);
            strcpy(locVariabile.str,"NULL");

            if(BSTSearch(symtable[FUNC_TABLE],pomStr.str) == NULL)
              BSTInsert(&symtable[FUNC_TABLE],pomStr.str,FALSE,NumberOfParams,NULL); // ak sa func nenajde tak pridame do TS ale poznacime ze nie je deklarovana

          }
          else if(token == EOL) {
            // neco = jednavec
            if(!(isDeclared(locVariabile.str)))
              Generate_assign_var(locVariabile.str);
            err = expression_precedence(token,locVariabile.str,pomStr.str);
          }

            //return SYN_OK;
          else
            return ERR_SYN;

          return err;
          break;
        }

      break; // END OF ID

    default:; // hocico ine

      int NumberOfParams = 0;
      if(token == NUMBER || token == STRING){
        if(!(isDeclared(locVariabile.str))){
          Generate_assign_var(locVariabile.str); //, attr, token);
        }
        err = expression_precedence(token,locVariabile.str,NULL);
      }
      else if(token > 79 && token < 83){ // inputy
        if(!(isDeclared(locVariabile.str))){
          Generate_assign_var(locVariabile.str);
        }
        Generate_read(token,locVariabile.str);
        if((token = getNextToken(&attr, &begin_str)) == ERR_LEX)
          return ERR_LEX;
        err = itemList(NULL,NULL);
        isLeft = false;
        return err;
      }
      else if(token > 82 && token < 88){ // vstavane funckie -> neukladame do TS
        if(strcmp("NULL",locVariabile.str))
            if(token != PRINT){
              if(!(isDeclared(locVariabile.str)))
                printf("DEFVAR LF@%s\n",locVariabile.str);
          }
        int funcType = token;
        printf("CREATEFRAME\n");
        token = getNextToken(&attr,&begin_str);
        err = itemList(&NumberOfParams,locVariabile.str);
        isLeft = false;
        if(err != SYN_OK)
          return err;
        err = Generate_built_in(funcType,NumberOfParams,locVariabile.str,indexToTS);
      }
      else
        return ERR_SYN;

      return err;

      break; // END
  }
}

int itemList(int *NumberOfParams,char *locVariabile){

  // na zaciatku si mozno ulozime prvy parameter pre pripad ze by bol jediny
  int err = SYN_OK;

  if(token == LEFT_BRACKET){
    if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
      return ERR_LEX;

    if(token == RIGHT_BRACKET)
      return SYN_OK;

    isLeft = TRUE;
  }
  if(token == EOL || token == RIGHT_BRACKET) // func a EOL or func () EOL
    return SYN_OK;

  if((token < 62 || token > 66) && token != LEFT_BRACKET)
    return ERR_SYN;

  if(indexToTS != 0){ // Ak sme neni v definovani funckie passujeme pa
    if(token == ID){
      tBSTNodePtr node;
      if((node = BSTSearch(symtable[LOCAL_V_TABLE],attr.str)) != NULL)
        Pass_func_parametres(NULL,*NumberOfParams,attr.str);

      else{
        BSTInsert(&symtable[LOCAL_V_TABLE],attr.str,FALSE,0,"nil");
        Pass_func_parametres("nil",*NumberOfParams,NULL);
      }
    }
    else
      Pass_func_parametres(attr.str,*NumberOfParams,NULL);

  }
  else{
    if(token == ID){
      tBSTNodePtr node;
      tBSTNodePtr nodeinTSF;
      if((node = BSTSearch(symtable[FUNC_TABLE],funcName.str)) != NULL){
        if((nodeinTSF = BSTSearch(node->TS,attr.str)) != NULL)
          Pass_func_parametres(NULL,*NumberOfParams,attr.str);
        else{
          BSTInsert(&node->TS,attr.str,FALSE,0,"nil");
          Pass_func_parametres("nil",*NumberOfParams,NULL);
        }
      }
    }
    else{
      if(readParams == true)
        return ERR_SYN;
      else
         Pass_func_parametres(attr.str,*NumberOfParams,NULL);
    }
  }

  if(readParams == true){
    passVariabile = (char**)malloc(sizeof(char*));
    if(passVariabile == NULL)
      return ERR_INTERN;

    passVariabile[*NumberOfParams] = (char*)malloc(sizeof(char)*(strlen(attr.str)+1));
    if(passVariabile[*NumberOfParams] == NULL)
      return ERR_INTERN;

    strcpy(passVariabile[*NumberOfParams],attr.str);
  }

  *NumberOfParams = *NumberOfParams + 1;
  err = itemList2(&(*NumberOfParams),locVariabile);

  return err;

}

int itemList2(int *NumberOfParams,char *locVariabile){

  int err = SYN_OK;
  if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
    return ERR_LEX;

  if(token != COMMA && token != RIGHT_BRACKET){// func (a b)
    if(token == EOL && isLeft != true)
      return SYN_OK;
    else
      return ERR_SYN;
  }

  while(token != RIGHT_BRACKET && token != EOL){

    if((token = getNextToken(&attr,&begin_str)) == ERR_LEX)
      return ERR_LEX;

    if(token < 62 ||   token > 66)
      return ERR_SYN;

    if(indexToTS != 0){ // Ak sme neni v definovani funckie passujeme pa

      if(token == ID){
        tBSTNodePtr node;
        if((node = BSTSearch(symtable[LOCAL_V_TABLE],attr.str)) != NULL)
          Pass_func_parametres(NULL,*NumberOfParams,attr.str);

        else{
          BSTInsert(&symtable[LOCAL_V_TABLE],attr.str,FALSE,0,"nil");
          Pass_func_parametres("nil",*NumberOfParams,NULL);
        }
      }
      else
        Pass_func_parametres(attr.str,*NumberOfParams,NULL);
    }
    else{
      if(token == ID){
        tBSTNodePtr node;
        tBSTNodePtr nodeinTSF;
        if((node = BSTSearch(symtable[FUNC_TABLE],funcName.str)) != NULL){
          if((nodeinTSF = BSTSearch(node->TS,attr.str)) != NULL)
            Pass_func_parametres(NULL,*NumberOfParams,attr.str);
          else{
            BSTInsert(&node->TS,attr.str,FALSE,0,"nil");
            Pass_func_parametres("nil",*NumberOfParams,NULL);
          }
        }
      }
      else{
        if(readParams == true)
          return ERR_SYN;
        else
           Pass_func_parametres(attr.str,*NumberOfParams,NULL);
      }
    }

    if(readParams == true){
      passVariabile = (char**)realloc(passVariabile,sizeof(char*)*(*NumberOfParams + 1));
      if(passVariabile == NULL)
        return ERR_INTERN;

      passVariabile[*NumberOfParams] = (char*)malloc(sizeof(char)*(strlen(attr.str)+1));
      if(passVariabile[*NumberOfParams] == NULL)
        return ERR_INTERN;
      strcpy(passVariabile[*NumberOfParams],attr.str);
    }
    *NumberOfParams = *NumberOfParams + 1;
    err = itemList2(&(*NumberOfParams),locVariabile);

    if(err != SYN_OK)
      return err;
  }

  if(token == RIGHT_BRACKET && isLeft != TRUE)
    return ERR_SYN;

  return err;

}
