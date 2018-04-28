/**
Toto je prvni projekt do predmetu IZP - zaklady programovani
Autor: xstepa60
**/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#define PRINTABLE_CHARS 95
#define MAX_ADDRESS_LENGTH 101

//Razeni znaku podle abecedy metodou bubblesort
void char_bubble_sort(char char_array[], int size);
//Funkce prevede pole malych znaku na pole velkych znaku
void convert_array_to_uppercase(char char_array[], int size);
//Funkce prevede pole znaku na same male znaky
void convert_array_to_lowercase(char *char_array, int size);
//Funkce zjisti, zda zadany prvek je obsazen v zadanem array
bool is_new_char(char char_array[], char c, int size);

int main(int argc, char *argv[]) {
  //Klavesy, ktere budou povoleny na virtualni klavesnici
  char avalaible_chars[PRINTABLE_CHARS];
  //Pocet jiz nalezenych znaku
  int founded_chars = 0;
  //V pripade nalezeni konkretni adresy se ulozi do tohoto pole znaku
  char address[MAX_ADDRESS_LENGTH];
  int founded_address_length = 0;
  //Ovlivni vypis programu, 
  int duplicitous = 0;
  //Hodnota true, pokud byla v aktualne prohledavanem radku nalezena shoda
  bool found = false;
  //Promenna start se da do stavu true na zacatku kazdeho noveho radku. Zabrani nam vyhledavani retezce uprostred slova.
  bool start = true;   
    
  if (argc == 2) {
    //Do tohoto pole charu se ulozi argument, oproti kteremu budeme kontrolovat shodu.
    char *specified_string = argv[1]; 
    //Zadany string v parametru se prevede na mala pismena, zajistuje case sensitivitu
    convert_array_to_lowercase(specified_string, (int) strlen(specified_string));
    //Pomocna promenna, vuci ni overuji hledane znaky se znaky na kazdem radku
    int index = 0;

    char c;
    //Nacitani znaku ze stdin a prevod na mala pismena
    while((c = tolower(getchar())) != EOF) {
        //Ulozi nasledujici pismeno, ktere bude povoleno na virtualni klavesnici
	if(((int) strlen(specified_string) == index) && (found == true)) {
	  //Pridani do pole, pokud tam jiz prvek neni
	  if(is_new_char(avalaible_chars, c, founded_chars)) { 
	    found = false;
	    avalaible_chars[founded_chars++] = c;
	    
	   //Nakopiruje do pole address to co je v specified_String + zbytek adresy do konce radku
	    for(int i = 0; i < (int) strlen(specified_string); i++) {
	      address[i] = specified_string[i];
	    }
	   
	    //Pokud budeme ukladat do pole address zbytek radku (prvne hledany parametr, pak zbytek radku), tak nam zajisti, abychom meli index pro ukladani do pole spravne nastaveny
	    founded_address_length = (int) strlen(specified_string);
	    //Delka zadaneho parametru a delka adresy na radku je totozna -> nalezena shoda a nasledujici znak je symbol noveho radku -> proto nepokracuj dal, v opacnem pripade nam getchar() "dojede" a ulozi zbytek adresy na radku do pole (pokud nastane case FOUND)
	    if(c == '\n') break; 
	    address[founded_address_length++] = c;  
	    int remaining_index = index;
	    while((c = tolower(getchar())) != '\n') {
	      address[++remaining_index] = c;
	      founded_address_length++;
	    }
	       	
	  } else { 
	    duplicitous++; 
	  } 
	}
	//Nastaveni podminek, aby se blok spustil na zacatku noveho radku a nebo pokud byla v predchozi iteraci nalezena shoda
	if(c == specified_string[index] && ((start == true  && found == false) || (start == false && found == true))) {
	  found = true;
	  start = false;
	  index += 1;
	}
	else if(c != specified_string[index] && (start == false && found == true)) {
	  start = false;
	  found = false;
	} else {
	  start = false;
	}
	//Zresetuje hodnoty po prechodu na novy radek	
	if(c == '\n') {
	  index = 0;
	  start = true;
	  found = false;
	}
    }

    if(founded_chars > 1 || duplicitous > 0) {
      char_bubble_sort(avalaible_chars, founded_chars);
      convert_array_to_uppercase(avalaible_chars, founded_chars);
      printf("Enable: ");
      for(int i = 0; i < founded_chars; i++) {
        printf("%c", avalaible_chars[i]);
      }
    } else if (founded_chars == 0) {
      printf("Not found");
    } else { //duplicitous 0
      convert_array_to_uppercase(address,founded_address_length);
      printf("Found: ");
      for(int i = 0; i < founded_address_length; i++) {
	printf("%c", address[i]);	
      }
    }
  //Tento blok se spusti v pripade, kdy nespecifikujeme adresu - nevlozime argument  
  } else if (argc == 1) {
      char c;      
      while((c = tolower(getchar())) != EOF){
	if(start == true) {
	  start = false;
	  if(is_new_char(avalaible_chars, c, founded_chars) && c != '\n') {
            avalaible_chars[founded_chars++] = c; 
	  }
        }
        if(c == '\n') {
	  start = true;
	}
      }
      char_bubble_sort(avalaible_chars, founded_chars);
      convert_array_to_uppercase(avalaible_chars, founded_chars);
      printf("Enable: ");
      for(int i = 0; i < founded_chars; i++) {
	printf("%c", avalaible_chars[i]); 
      }
   } else {
      fprintf(stderr,"Too many arguments!");
  }
  printf("\n");
  return 0;
}

void char_bubble_sort(char char_array[], int size) {
  for(int i = 0; i < size - 1; i++) {
    for(int j = 0; j < size - i - 1; j++) {
      if(char_array[j+1] < char_array[j]) {
        int pom = char_array[j+1];
        char_array[j+1] = char_array[j];
        char_array[j] = pom;
      }
    }
  }
}

void convert_array_to_uppercase(char char_array[], int size) {
  for(int i = 0; i < size; i++) {
    char_array[i] = toupper(char_array[i]);
  }
}

void convert_array_to_lowercase(char *char_array, int size) {
  for(int i= 0; i < size; i++) {
    char_array[i] = tolower(char_array[i]);
  }

}

bool is_new_char(char char_array[], char c, int size) {
  for(int i = 0; i < size; i++) {
    if(char_array[i] == c) {
      return false;
    }
  }
  return true;
}
