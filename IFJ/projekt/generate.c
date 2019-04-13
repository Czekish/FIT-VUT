/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */
#include "generate.h"
static int indexToTS = 1;

bool isInt(char *string){

  for(size_t i=0; i<strlen(string); i++){
    if(string[i] < 48 || string[i] > 57)
      return false;
    }

    return true;

}

bool isFloat(char *string){

  char *p;
  strtod(string,&p);
  if (strlen(string)==0) {
      return false;
  } else {
      if (*p == 0) {
          return true;
      } else {
          return false;
      }
  }
}

int reverseNumber(int n){

  int reversedNumber = 0;
  while(n != 0)
  {
      int remainder = n%10;
      reversedNumber = reversedNumber*10 + remainder;
      n /= 10;
  }
  return reversedNumber;

}

int fillString(char **str,char c){

  int i = 0;

  if(*str){
    while((*str)[i] != '\0')
      i++;
  }

  if(c < 33 || c == 35 || c == 92){
    (*str)[i] = 92;
    (*str)[i+1] = '\0';
    i++;
    (*str)[i] = '0';
    i++;

    int reversedNumber = 0;
    reversedNumber = reverseNumber(c);

    while(reversedNumber != 0){
      int remainder = reversedNumber%10;
      reversedNumber = reversedNumber/10;

      (*str)[i] = (char)(remainder + 48);
      i++;
    }

    if((c % 10) == 0){
      (*str)[i] = 48;
      i++;
    }

    (*str)[i] ='\0';
    return 0;
  }



  (*str)[i] = c;
  (*str)[i+1] ='\0';
  return 0;
}

int whiteCharsCount(char *string,int len){

  int count = 0;
  for (int i=0; i<len; i++){
    if(string[i] < 33 || string[i] == 35 || string[i] == 92)
      count+=1;
  }
  return count;
}


void toRightString(char *string,char *variabile,int flag,int indexToTS){

    int len = strlen(string);
    int whiteChars = whiteCharsCount(string,len);
    char *str = (char *)malloc(sizeof(char)*(len + whiteChars*4 + 1));

    if(string == NULL || str == NULL){
      if(str){
        free(str);
        str = NULL;
      }
      return ;
    }


    str[0] = '\0';
    for (int i=0; i<len; i++){
      fillString(&str,string[i]);

    }
    if(flag >= 0)
      printf("MOVE TF@%s%d string@%s\n","%",flag,str);
    else if (flag == -2){
      printf("PUSHS string@%s\n",str);
      //free(str);
      //str = NULL;
    }
    else{
      printf("MOVE LF@%s string@%s\n",variabile,str);
      if(indexToTS == FUNC_TABLE)
        printf("MOVE LF@%sretval LF@%s\n","%",variabile);
    }

    if (str){
      free(str);
      str = NULL;
    }
}

int getType(char *string){

  if(isInt(string))
    return 0;
  else if(isFloat(string))
    return 1;
  else
    return 2;
}

void Generate_start_file(){

  printf(".IFJcode18\n");
  printf("CREATEFRAME\nPUSHFRAME\n");

}

void Generate_func_start(char *string,int NumberOfParams,char **passVariabile){
  indexToTS = 0;
  printf("JUMP $%sEND\n",string);
  printf("LABEL $%s\n",string);
  printf("PUSHFRAME\n");
  printf("DEFVAR LF@%sretval\n","%");
  printf("MOVE LF@%sretval nil@nil\n","%");

  for(int i=0; i<NumberOfParams; i++){
    if(passVariabile[i] != NULL){
      printf("DEFVAR LF@%s\n",passVariabile[i]);
      printf("MOVE LF@%s LF@%s%d\n",passVariabile[i],"%",i);
    }
    else{
      printf("DEFVAR LF@param%d\n",i);
      printf("MOVE LF@param%d LF@%s%d\n",i,"%",i);
    }
  }
}

void Generate_func_end(char *string){
  indexToTS = 1;
  printf("POPFRAME\n");
  printf("RETURN\n\n");
  printf("LABEL $%sEND\n",string);
}

void Pass_func_parametres(char *string,int paramCount,char *locVariabile){
  printf("DEFVAR TF@%s%d\n","%",paramCount);
  if(string == NULL){
    printf("MOVE TF@%s%d LF@%s\n","%",paramCount,locVariabile);
    return;
  }
  switch(getType(string)){

    case 0: // int
      printf("MOVE TF@%s%d int@%s\n","%",paramCount,string);
      break;

    case 1: // float
      printf("MOVE TF@%s%d float@%a\n","%",paramCount,atof(string));
      break;

    case 2: ;// string
      toRightString(string,NULL,paramCount,NO_TS);
      break;

    default:
      break;
  }
  return;
}

void Generate_assign_func_var(char *variabile){
  if(strcmp(variabile,"NULL")){
    printf("DEFVAR LF@%s\nCREATEFRAME\n",variabile);
  }
}

void Generate_to_func_var(char *variabile,int indexToTS){
  if(strcmp(variabile,"NULL"))
    printf("MOVE LF@%s TF@%sretval\n",variabile,"%");
  if(indexToTS == FUNC_TABLE)
    printf("MOVE LF@%sretval LF@%s\n","%",variabile);


}

void Generate_assign_var(char *variabile){ //, string attr, int token){

    printf("DEFVAR LF@%s\n",variabile);
}

void Generate_final_move_int_var(char *variabile) {
  printf("POPS LF@%s\n", variabile);
  if(indexToTS == 0)
    printf("MOVE LF@%sretval LF@%s\n","%",variabile );
}

void Generate_stack_dividing(int random) {

    printf("# Pred delenim jdeme kontrolovat hodnoty\n");

    printf("DEFVAR LF@a$tmp%d\n", random);
    printf("DEFVAR LF@b$tmp%d\n", random);

    printf("DEFVAR LF@a$type%d\n", random);
    printf("DEFVAR LF@b$type%d\n", random);

    printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
    printf("POPS LF@a$tmp%d\n", random);
    printf("POPS LF@b$tmp%d\n", random);

    printf("# Pretypovani\n");
    printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
    printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

    printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);
    printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);
    printf("JUMPIFEQ $sem$err%d LF@a$type%d string@string\n",random, random);

    printf("LABEL $foo$if$a$float%d\n", random);
    printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
    printf("JUMPIFEQ $div$zero%d LF@a$tmp%d float@0x0p+0\n", random, random);
    printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
    printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

    printf("PUSHS LF@b$tmp%d\n", random);
    printf("PUSHS LF@a$tmp%d\n", random);
    printf("DIVS\n");
    printf("JUMP $leave%d\n", random);

    printf("LABEL $foo$if$a$int%d\n", random);
    printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
    printf("JUMPIFEQ $div$zero%d LF@a$tmp%d int@0\n", random, random);
    printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
    printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

    printf("PUSHS LF@b$tmp%d\n", random);
    printf("PUSHS LF@a$tmp%d\n", random);
    printf("DIVS\n");
    printf("JUMP $leave%d\n", random);

    printf("LABEL $foo$if$b$float%d\n", random);
    printf("PUSHS LF@b$tmp%d\n", random);
    printf("PUSHS LF@a$tmp%d\n", random);
    printf("DIVS\n");
    printf("JUMP $leave%d\n", random);

    printf("LABEL $foo$if$b$int%d\n", random);
    printf("PUSHS LF@b$tmp%d\n", random);
    printf("PUSHS LF@a$tmp%d\n", random);
    printf("IDIVS\n");
    printf("JUMP $leave%d\n", random);

    printf("LABEL $div$zero%d\n", random);
    printf("DPRINT string@Doslo\\032k\\032deleni\\032nulou!!\n");
    printf("EXIT int@%d\n", ERR_ZERO_DIVISION);

    printf("LABEL $sem$err%d\n", random);
    printf("DPRINT string@Nekompatibilni\\032datove\\032typy!!\n");
    printf("EXIT int@%d\n", ERR_SEM_TYPE);

    printf("LABEL $leave%d\n", random);
}

void Generate_stack_adds(int random) {

  printf("# Pred scitanim jdeme kontrolovat hodnoty\n");

  printf("DEFVAR LF@a$tmp%d\n", random);
  printf("DEFVAR LF@b$tmp%d\n", random);

  printf("DEFVAR LF@a$type%d\n", random);
  printf("DEFVAR LF@b$type%d\n", random);

  printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
  printf("POPS LF@a$tmp%d\n", random);
  printf("POPS LF@b$tmp%d\n", random);

  printf("# Pretypovani\n");
  printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
  printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

  printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);
  printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);
  printf("JUMPIFEQ $foo$if$a$string%d LF@a$type%d string@string\n",random, random);

  printf("LABEL $foo$if$a$string%d\n", random);
  printf("JUMPIFEQ $foo$if$b$string%d LF@b$type%d string@string\n",random, random);
  printf("JUMP $sem$err%d\n", random);

  printf("LABEL $foo$if$b$string%d\n", random);
  printf("DEFVAR LF@c$tmp%d\n", random);
  printf("CONCAT LF@c$tmp%d LF@b$tmp%d LF@a$tmp%d\n", random, random, random);
  printf("PUSHS LF@c$tmp%d\n", random);
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$a$float%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
  printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("ADDS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$a$int%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
  printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("ADDS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$b$float%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("ADDS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$b$int%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("ADDS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $sem$err%d\n", random);
  printf("DPRINT string@Nekompatybilni\\032datove\\032typy!!\n");
  printf("EXIT int@%d\n", ERR_SEM_TYPE);

  printf("LABEL $leave%d\n", random);

}
void Generate_stack_muls(int random) {
  //printf("MULS\n");

  printf("# Pred nasobenim jdeme kontrolovat hodnoty\n");

  printf("DEFVAR LF@a$tmp%d\n", random);
  printf("DEFVAR LF@b$tmp%d\n", random);

  printf("DEFVAR LF@a$type%d\n", random);
  printf("DEFVAR LF@b$type%d\n", random);

  printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
  printf("POPS LF@a$tmp%d\n", random);
  printf("POPS LF@b$tmp%d\n", random);

  printf("# Pretypovani\n");
  printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
  printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

  printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);
  printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);
  printf("JUMPIFEQ $sem$err%d LF@a$type%d string@string\n",random, random);

  printf("LABEL $foo$if$a$float%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
  printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("MULS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$a$int%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
  printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("MULS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$b$float%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("MULS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$b$int%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("MULS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $sem$err%d\n", random);
  printf("DPRINT string@Nekompatibilni\\032datove\\032typy!!\n");
  printf("EXIT int@%d\n", ERR_SEM_TYPE);

  printf("LABEL $leave%d\n", random);
}
void Generate_stack_subs(int random){
  //printf("SUBS\n");


  printf("# Pred odcitanim jdeme kontrolovat hodnoty\n");

  printf("DEFVAR LF@a$tmp%d\n", random);
  printf("DEFVAR LF@b$tmp%d\n", random);

  printf("DEFVAR LF@a$type%d\n", random);
  printf("DEFVAR LF@b$type%d\n", random);

  printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
  printf("POPS LF@a$tmp%d\n", random);
  printf("POPS LF@b$tmp%d\n", random);

  printf("# Pretypovani\n");
  printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
  printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

  printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);
  printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);
  printf("JUMPIFEQ $sem$err%d LF@a$type%d string@string\n",random, random);


  printf("LABEL $foo$if$a$float%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
  printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("SUBS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$a$int%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
  printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("SUBS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$b$float%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("SUBS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $foo$if$b$int%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("SUBS\n");
  printf("JUMP $leave%d\n", random);

  printf("LABEL $sem$err%d\n", random);
  printf("DPRINT string@Nekompatibilni\\032datove\\032typy!!\n");
  printf("EXIT int@%d\n", ERR_SEM_TYPE);

  printf("LABEL $leave%d\n", random);

}


void Generate_to_var(char *variabile,char *value,int indexToTS){
  switch(getType(value)){

    case 0: // int
      printf("MOVE LF@%s int@%s\n",variabile,value);
      if(indexToTS == FUNC_TABLE)
        printf("MOVE LF@%sretval LF@%s\n","%",variabile);
      break;

    case 1: // float
      printf("MOVE LF@%s float@%a\n",variabile,atof(value));
      if(indexToTS == FUNC_TABLE)
        printf("MOVE LF@%sretval LF@%s\n","%",variabile);
      break;

    case 2: ;// string
      toRightString(value,variabile,-1,indexToTS);
      break;

    default:
      break;

  }
  return;
}

int Generate_built_in(int token,int NumberOfParams,char *locVariabile,int indexToTS){

  static int counterif = 0;
  static int counterend = 0;

  printf("PUSHFRAME\n");
  switch(token){

    case PRINT:
      //(void)locVariabile;
      for(int i=0; i<NumberOfParams; i++){
        printf("WRITE LF@%s%d\n","%",i);
      }
    break;

    case LENGTH:
      if(NumberOfParams != 1)
        return ERR_SEM;
      printf("DEFVAR LF@type\n");
      printf("TYPE LF@type LF@%s0\n","%");
      printf("JUMPIFEQ $length%d LF@type string@string\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $length%d\n",counterif);
      counterif+=1;
      printf("DEFVAR LF@%sretval\n","%");
      printf("STRLEN LF@%sretval LF@%s%d\n","%","%",0);
      break;

    case SUBSTR:
      if(NumberOfParams != 3)
        return ERR_SEM;

      for(int i=0; i<NumberOfParams; i++){
        printf("DEFVAR LF@param%d\n",i);
        printf("MOVE LF@param%d LF@%s%d\n",i,"%",i);
      }
      printf("DEFVAR LF@type\n");
      printf("TYPE LF@type LF@%s0\n","%");
      printf("JUMPIFEQ $substr%d LF@type string@string\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $substr%d\n",counterif);
      counterif+=1;
      printf("TYPE LF@type LF@%s1\n","%");
      printf("JUMPIFEQ $substr%d LF@type string@int\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $substr%d\n",counterif);
      counterif+=1;
      printf("TYPE LF@type LF@%s2\n","%");
      printf("JUMPIFEQ $substr%d LF@type string@int\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $substr%d\n",counterif);
      counterif+=1;

      printf("DEFVAR LF@%sretval\nMOVE LF@%sretval string@\n","%","%");
      printf("DEFVAR LF@pom\n");
      printf("GT LF@pom LF@param1 int@-1\n");
      printf("JUMPIFEQ $%strue%d LF@pom bool@true\n",locVariabile,counterif);
      printf("MOVE LF@%sretval string@nil\n","%");
      printf("JUMP %s%dend\n",locVariabile,counterend);
      printf("LABEL $%strue%d\n",locVariabile,counterif);
      counterif+=1;
      printf("DEFVAR LF@%slen\nSTRLEN LF@%slen LF@param0\n","%","%");
      printf("SUB LF@%slen LF@%slen LF@param1\n","%","%");
      printf("JUMPIFEQ $%strue%d LF@%slen LF@param2\n",locVariabile,counterif,"%");
      printf("GT LF@pom LF@param2 LF@%slen\n","%");
      printf("JUMPIFEQ $%strue%d LF@pom bool@false\n",locVariabile,counterif);
      printf("MOVE LF@param2 LF@%slen\n","%");
      printf("LABEL $%strue%d\n",locVariabile,counterif);
      counterif+=1;
      printf("DEFVAR LF@getChar\n");
      printf("DEFVAR LF@counter\n");
      printf("MOVE LF@counter int@1\n");
      printf("LABEL %s%dstart\n",locVariabile,counterend);
      printf("GETCHAR LF@getChar LF@param0 LF@param1\n");
      printf("CONCAT LF@%sretval LF@%sretval LF@getChar\n","%","%");
      printf("JUMPIFEQ %s%dend LF@counter LF@param2\n",locVariabile,counterend);
      printf("ADD LF@counter  LF@counter int@1\n");
      printf("ADD LF@param1  LF@param1 int@1\n");
      printf("JUMP %s%dstart \n",locVariabile,counterend);
      printf("LABEL %s%dend\n",locVariabile,counterend);
      counterend += 1;

    break;

    case ORD:
      if(NumberOfParams != 2)
        return ERR_SEM;

      for(int i=0; i<NumberOfParams; i++){
        printf("DEFVAR LF@param%d\n",i);
        printf("MOVE LF@param%d LF@%s%d\n",i,"%",i);
      }
      printf("DEFVAR LF@type\n");
      printf("TYPE LF@type LF@%s0\n","%");
      printf("JUMPIFEQ $ord%d LF@type string@string\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $ord%d\n",counterif);
      counterif+=1;
      printf("TYPE LF@type LF@%s1\n","%");
      printf("JUMPIFEQ $ord%d LF@type string@int\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $ord%d\n",counterif);
      counterif+=1;

      printf("DEFVAR LF@%sretval\nMOVE LF@%sretval string@\n","%","%");
      printf("DEFVAR LF@pom\n");
      printf("DEFVAR LF@%slen\nSTRLEN LF@%slen LF@param0\n","%","%");
      printf("GT LF@pom LF@param1 int@-1\n");
      printf("JUMPIFEQ $%s%dtrue LF@pom bool@true\n",locVariabile,counterif);
      printf("MOVE LF@%sretval string@nil\n","%");
      printf("JUMP $%s%dend\n",locVariabile,counterend);
      printf("LABEL $%s%dtrue\n",locVariabile,counterif);
      counterif+=1;
      printf("LT LF@pom LF@param1 LF@%slen\n","%");
      printf("JUMPIFEQ $%s%dstart LF@pom bool@true\n",locVariabile,counterif);
      printf("MOVE LF@%sretval string@nil\n","%");
      printf("JUMP $%s%dend\n",locVariabile,counterend);
      printf("LABEL $%s%dstart\n",locVariabile,counterif);
      counterif+=1;
      printf("STRI2INT LF@%sretval LF@param0 LF@param1\n","%");
      printf("LABEL $%s%dend\n",locVariabile,counterend);
      counterend+=1;

    break;

    case CHR:
      if(NumberOfParams != 1)
        return ERR_SEM;

      for(int i=0; i<NumberOfParams; i++){
        printf("DEFVAR LF@param%d\n",i);
        printf("MOVE LF@param%d LF@%s%d\n",i,"%",i);
      }
      printf("DEFVAR LF@type\n");
      printf("TYPE LF@type LF@%s0\n","%");
      printf("JUMPIFEQ $chr%d LF@type string@int\n",counterif);
      printf("EXIT int@6\n");
      printf("LABEL $chr%d\n",counterif);
      counterif+=1;

      printf("TYPE LF@type LF@param0\n");
      printf("JUMPIFEQ $%s%dtrue LF@type string@int\n",locVariabile,counterif);
      printf("EXIT int@6\n");
      printf("LABEL $%s%dtrue\n",locVariabile,counterif);
      counterif+=1;
      printf("DEFVAR LF@pom\n");
      printf("GT LF@pom LF@param0 int@-1\n");
      printf("JUMPIFEQ $%s%dtrue LF@pom bool@true\n",locVariabile,counterif);
      printf("EXIT int@6\n");
      printf("LABEL $%s%dtrue\n",locVariabile,counterif);
      counterif+=1;
      printf("LT LF@pom LF@param0 int@256\n");
      printf("JUMPIFEQ $%s%dtrue LF@pom bool@true\n",locVariabile,counterif);
      printf("EXIT int@6\n");
      printf("LABEL $%s%dtrue\n",locVariabile,counterif);
      counterif+=1;
      printf("DEFVAR LF@%sretval\nMOVE LF@%sretval string@\n","%","%");
      printf("INT2CHAR LF@%sretval LF@param0\n","%");

    break;
    }
    printf("POPFRAME\n");
    if(strcmp(locVariabile,"NULL") && token != PRINT){
      printf("MOVE LF@%s TF@%sretval\n",locVariabile,"%");
    }
    if(indexToTS == FUNC_TABLE && token != PRINT)
      printf("MOVE LF@%sretval LF@%s\n","%",locVariabile);

    return SYN_OK;
}

void Generate_push_number(char *val_str) {

    if (isInt(val_str)) {
      printf("PUSHS int@%d\n", atoi(val_str));
    }else if (isFloat(val_str)) {
      printf("PUSHS float@%a\n", atof(val_str));
    }
}

void Generate_push_string(char *val_str){
    toRightString(val_str, NULL, -2, NO_TS);
}

void Generate_push_LF(char *print) {
  printf("PUSHS LF@%s\n", print);
}

void Generate_read(int token,char *locVariabile){

  printf("CREATEFRAME\nDEFVAR TF@%sretval\nPUSHFRAME\n","%" );
  switch(token){

    case INPUTS:
      printf("READ LF@%sretval string\n","%");
      break;

    case INPUTF:
      printf("READ LF@%sretval float\n","%");
      break;

    case INPUTI:
      printf("READ LF@%sretval int\n","%");
      break;

    default:

      break;

  }

  printf("POPFRAME\n");
  if(strcmp(locVariabile,"NULL"))
    printf("MOVE LF@%s TF@%sretval\n",locVariabile,"%");

}

void Generate_less_great(int random, int less_or_great) {
  printf("DEFVAR LF@a$tmp%d\n", random);
  printf("DEFVAR LF@b$tmp%d\n", random);
  /// Promena pro ulozeni vysledku
  printf("DEFVAR LF@result%d\n", random);


  printf("DEFVAR LF@a$type%d\n", random);
  printf("DEFVAR LF@b$type%d\n", random);

  printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
  printf("POPS LF@a$tmp%d\n", random);
  printf("POPS LF@b$tmp%d\n", random);

  printf("# Ulozeni typu do promennych\n");
  printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
  printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

  // V A je cislo napravo vyrazu
  // V B je cislo nalevo vyrazu

  /// ************************* OSETRENI PODMINEK *************************
  printf("# Skaceme v pripade, ze A je float\n");
  printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);

  printf("# Skaceme v pripade, ze A je int\n");
  printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);

  printf("# Skaceme v pripade, ze A je string\n");
  printf("JUMPIFEQ $foo$if$a$string%d LF@a$type%d string@string\n",random, random);

  /// ************************* LABEL A JE STRING *************************
  printf("LABEL $foo$if$a$string%d\n", random);
  printf("JUMPIFEQ $foo$if$b$string%d LF@b$type%d string@string\n",random, random);
  printf("JUMP $sem$err%d\n", random);

  /// ************************* LEXIKOGRAFICKE POROVNANI STRINGU **********
  printf("LABEL $foo$if$b$string%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (less_or_great == LESS) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }
  // V promenne LF@resultXXXX je pravdivostni hodnota, v pripade false skocime na navesti else
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL A JE FLOAT **************************
  printf("LABEL $foo$if$a$float%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
  printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

  /// ************************* A a B (po pretypovani) JSOU FLOAT *********
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (less_or_great == LESS) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* A JE INTEGER *****************************
  printf("LABEL $foo$if$a$int%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
  /// Pretypovani, protoze A byl INTEGER a B byl FLOAT
  printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

  /// ************************* B BYL INTEGER, POROVNANI *****************
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (less_or_great == LESS) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL A a B JSOU FLOAT *******************
  printf("LABEL $foo$if$b$float%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (less_or_great == LESS) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);


  /// ************************** LABEL B JE INT **************************
  printf("LABEL $foo$if$b$int%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (less_or_great == LESS) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL DOSLO K SEMANTICKE CHYBE ************
  printf("LABEL $sem$err%d\n", random);
  printf("DPRINT string@Nekompatibilni\\032datove\\032typy!!\n");
  printf("EXIT int@%d\n", ERR_SEM_TYPE);

  /// ************************* LABEL PRO KOREKTNI UKONCENI ***************
  printf("LABEL $leave%d\n", random);



}

void Generate_less_great_eq(int random, int rel_op) {
  printf("DEFVAR LF@a$tmp%d\n", random);
  printf("DEFVAR LF@b$tmp%d\n", random);
  /// Promena pro ulozeni vysledku
  printf("DEFVAR LF@result%d\n", random);


  printf("DEFVAR LF@a$type%d\n", random);
  printf("DEFVAR LF@b$type%d\n", random);

  printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
  printf("POPS LF@a$tmp%d\n", random);
  printf("POPS LF@b$tmp%d\n", random);

  printf("# Ulozeni typu do promennych\n");
  printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
  printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

  // V A je cislo napravo vyrazu
  // V B je cislo nalevo vyrazu

  /// ************************* OSETRENI PODMINEK *************************
  printf("# Skaceme v pripade, ze A je float\n");
  printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);

  printf("# Skaceme v pripade, ze A je int\n");
  printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);

  printf("# Skaceme v pripade, ze A je string\n");
  printf("JUMPIFEQ $foo$if$a$string%d LF@a$type%d string@string\n",random, random);

  /// ************************* LABEL A JE STRING *************************
  printf("LABEL $foo$if$a$string%d\n", random);
  printf("JUMPIFEQ $foo$if$b$string%d LF@b$type%d string@string\n",random, random);
  printf("JUMP $sem$err%d\n", random);

  /// ************************* LEXIKOGRAFICKE POROVNANI STRINGU **********
  printf("LABEL $foo$if$b$string%d\n", random);

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (rel_op == LESS_OR_EQUAL) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("EQS\n");

  printf("ORS\n");

  // V promenne LF@resultXXXX je pravdivostni hodnota, v pripade false skocime na navesti else
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL A JE FLOAT **************************
  printf("LABEL $foo$if$a$float%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
  printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

  /// ************************* A a B (po pretypovani) JSOU FLOAT *********
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  if (rel_op == LESS_OR_EQUAL) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("EQS\n");

  printf("ORS\n");

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* A JE INTEGER *****************************
  printf("LABEL $foo$if$a$int%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
  /// Pretypovani, protoze A byl INTEGER a B byl FLOAT
  printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

  /// ************************* B BYL INTEGER, POROVNANI *****************
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == LESS_OR_EQUAL) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("EQS\n");

  printf("ORS\n");

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL A a B JSOU FLOAT *******************
  printf("LABEL $foo$if$b$float%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == LESS_OR_EQUAL) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("EQS\n");

  printf("ORS\n");

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);


  /// ************************** LABEL B JE INT **************************
  printf("LABEL $foo$if$b$int%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == LESS_OR_EQUAL) {
    printf("LTS\n");
  } else {
    printf("GTS\n");
  }

  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);
  printf("EQS\n");

  printf("ORS\n");

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL DOSLO K SEMANTICKE CHYBE ************
  printf("LABEL $sem$err%d\n", random);
  printf("DPRINT string@Nekompatibilni\\032datove\\032typy!!\n");
  printf("EXIT int@%d\n", ERR_SEM_TYPE);

  /// ************************* LABEL PRO KOREKTNI UKONCENI ***************
  printf("LABEL $leave%d\n", random);

}

void Generate_eq(int random, int rel_op) {
  printf("DEFVAR LF@a$tmp%d\n", random);
  printf("DEFVAR LF@b$tmp%d\n", random);
  /// Promena pro ulozeni vysledku
  printf("DEFVAR LF@result%d\n", random);


  printf("DEFVAR LF@a$type%d\n", random);
  printf("DEFVAR LF@b$type%d\n", random);

  printf("# Ulozeni promennych ze stacku do docasnych promennych\n");
  printf("POPS LF@a$tmp%d\n", random);
  printf("POPS LF@b$tmp%d\n", random);

  printf("# Ulozeni typu do promennych\n");
  printf("TYPE LF@a$type%d LF@a$tmp%d\n",random, random);
  printf("TYPE LF@b$type%d LF@b$tmp%d\n",random, random);

  // V A je cislo napravo vyrazu
  // V B je cislo nalevo vyrazu

  /// ************************* OSETRENI PODMINEK *************************
  printf("# Skaceme v pripade, ze A je float\n");
  printf("JUMPIFEQ $foo$if$a$float%d LF@a$type%d string@float\n",random, random);

  printf("# Skaceme v pripade, ze A je int\n");
  printf("JUMPIFEQ $foo$if$a$int%d LF@a$type%d string@int\n",random, random);

  printf("# Skaceme v pripade, ze A je string\n");
  printf("JUMPIFEQ $foo$if$a$string%d LF@a$type%d string@string\n",random, random);

  /// ************************* LABEL A JE STRING *************************
  printf("LABEL $foo$if$a$string%d\n", random);
  printf("JUMPIFEQ $foo$if$b$string%d LF@b$type%d string@string\n",random, random);
  printf("JUMP $sem$err%d\n", random);

  /// ************************* LEXIKOGRAFICKE POROVNANI STRINGU **********
  printf("LABEL $foo$if$b$string%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == COMPARE) {
    printf("EQS\n");
  } else {
    printf("EQS\n");
    printf("NOTS\n");
  }

  // V promenne LF@resultXXXX je pravdivostni hodnota, v pripade false skocime na navesti else
  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL A JE FLOAT **************************
  printf("LABEL $foo$if$a$float%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$float%d LF@b$type%d string@float\n",random, random);
  printf("INT2FLOAT LF@b$tmp%d LF@b$tmp%d\n", random, random);

  /// ************************* A a B (po pretypovani) JSOU FLOAT *********
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == COMPARE) {
    printf("EQS\n");
  } else {
    printf("EQS\n");
    printf("NOTS\n");
  }

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* A JE INTEGER *****************************
  printf("LABEL $foo$if$a$int%d\n", random);
  printf("JUMPIFEQ $sem$err%d LF@b$type%d string@string\n",random, random);
  printf("JUMPIFEQ $foo$if$b$int%d LF@b$type%d string@int\n",random, random);
  /// Pretypovani, protoze A byl INTEGER a B byl FLOAT
  printf("INT2FLOAT LF@a$tmp%d LF@a$tmp%d\n", random, random);

  /// ************************* B BYL INTEGER, POROVNANI *****************
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == COMPARE) {
    printf("EQS\n");
  } else {
    printf("EQS\n");
    printf("NOTS\n");
  }

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL A a B JSOU FLOAT *******************
  printf("LABEL $foo$if$b$float%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == COMPARE) {
    printf("EQS\n");
  } else {
    printf("EQS\n");
    printf("NOTS\n");
  }

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);


  /// ************************** LABEL B JE INT **************************
  printf("LABEL $foo$if$b$int%d\n", random);
  printf("PUSHS LF@b$tmp%d\n", random);
  printf("PUSHS LF@a$tmp%d\n", random);

  if (rel_op == COMPARE) {
    printf("EQS\n");
  } else {
    printf("EQS\n");
    printf("NOTS\n");
  }

  printf("POPS LF@result%d\n", random);

  /// ************************* V PRIPADE FALSE SKOCI DO FALSE VETVE ******
  printf("JUMPIFEQ $else%d LF@result%d bool@false\n", random, random);
  printf("JUMP $leave%d\n", random);

  /// ************************* LABEL DOSLO K SEMANTICKE CHYBE ************
  printf("LABEL $sem$err%d\n", random);
  //printf("DPRINT string@Nekompatibilni\\032datove\\032typy!!\n");
  // Pri nemoznosti konverze je vysledek prohlasen za nepravdu
  printf("JUMP $else%d\n", random);

  /// ************************* LABEL PRO KOREKTNI UKONCENI ***************
  printf("LABEL $leave%d\n", random);

}
