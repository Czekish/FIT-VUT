#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#define DEFAULT_HEIGHT 1.5

//Prototypy funkci
//Detailni vysvetleni je uvedeno u definice kazde funkce
double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);
void print_help(void);
void compare_tangens(double angle, int N, int M);
int find_arg_index(char *arg,int argc, char **argv);
double calculate_distance(double angle, double height);
double calculate_object_height(double angle, double distance);
int get_iterations(double angle); 
void angle_err(void);
int check_number_of_iterations(int left, int right);
void iter_num_err(void);
int check_height_interval(double height);
void height_range_err(void);

int main(int argc, char **argv) {
  if(argc <= 1) {
    fprintf(stderr, "No arguments passed!\n");
    print_help();
    exit(1);
  } else {
    if(strcmp(argv[1],"--help") == 0) {
      print_help();
      exit(0);
    } else if(strcmp(argv[1], "--tan") == 0) {
      double angle = strtod(argv[2], NULL);
      int left_interval = atoi(argv[3]);
      int right_interval = atoi(argv[4]);
      if(check_number_of_iterations(left_interval, right_interval)) {
        compare_tangens(angle, left_interval, right_interval);
      } else {
        iter_num_err();
	exit(0);
      }
    } else {
      double height = (strcmp(argv[1], "-c") == 0) ? strtod(argv[2], NULL) : DEFAULT_HEIGHT;
      double distance = calculate_distance(strtod(argv[(find_arg_index("-m", argc, argv) + 1)],NULL), height);
      double total_height;

      if(check_height_interval(height)) {
        printf("%.10e\n", distance);
      } else {
        height_range_err();
        exit(0);
      }

      if (argc == 4 || argc == 6) {
        total_height = height + calculate_object_height(strtod(argv[(find_arg_index("-m", argc, argv) + 2)], NULL), distance); 
        printf("%.10e\n", total_height);
      }
    } 
  }
  
  return 0;
}

//Implementace funkce tangens pomoci metody Taylorova polynomu
//Funkce prebira dva argumenty, doube - uhel v rad, a unsigned int - pocet iteraci/rozvoj polynomu
double taylor_tan(double x, unsigned int n) {
  //Konstanty pro Tayloruv polynom
  double numerators[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
  double denominators[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
  double angle = x;
  double sum = 0;
  unsigned int i;

  for(i = 0; i < n; i++) {
    sum += (angle*numerators[i])/denominators[i];
    angle *= (x*x);
  }
  return sum;
}
//Implementace funkce tangens pomoci metody zretezenych zlomku
//Funkce prebira dva argumnty, double - uhel v rad, a unsigned int - pocet iteraci
double cfrac_tan(double x, unsigned int n) {
  double cf = 0.0;
  double a, b;

  for(int k = n-1; k >= 0; k--) {
    a = (2*k+1)/x;
    b = 1.0;
    cf = b / (a - cf);
  }
  return cf;
}

void print_help(void) {
  printf("%s\n%s\n%s\n%s\n","Daniel Stepan (xstepa60)", "./proj2 --help - napoveda", "./proj2 --tan A N M (A - uhel v rad, N - levy interval, M - pravy interval)", "./proj2 [-c X] -m A [B] (X - vyska pristroje, A - uhel alpha v rad, B - uhel beta v rad)");
}
//Funkce porovna a vypise tangensy v zadanem formatu
void compare_tangens(double angle, int N, int M) {
  for(int i = N; i <= M; i++) {
      printf("%d %e %e %e %e %e\n", 
         i, 
         tan(angle), 
         taylor_tan(angle,i), 
         fabs(tan(angle)-taylor_tan(angle,i)), 
         cfrac_tan(angle, i), 
         fabs(tan(angle)-cfrac_tan(angle,i)) 
      );
  }
}
//Funkce nalezna na ktere pozici byl zadan konkretni argument
//Funkce prebira tri parametry typu hledane pole znaku, pocet argumentu programu a ukazatel na toto pole
int find_arg_index(char *arg, int argc, char **argv) {
  for(int i = 0; i < argc; i++) {
    if(strcmp(argv[i], arg) == 0)
      return i;
  }
  return -1;
}
//Funkce spocita vzdalenost objektu
//Pro kazdy uhel z intervalu 0.0-1.4 rad je potreba jiny minimalni pocet iteraci k dostatecne presnosti
//Funkce prebira dva parametry typu double - uhel v rad a vzdalenost
double calculate_distance(double angle, double height) {
  unsigned int iter = get_iterations(angle);
  return height/cfrac_tan(angle, iter); 
}
//Funkce vypocita vysku objektu pomoci goniometricke funkce tangens implementovane metodou zretezenych zlomku
//Funkce prebira dva parametry typu double - uhel v rad a vzdalenost
double calculate_object_height(double angle, double distance) {
  unsigned int iter = get_iterations(angle);
  return cfrac_tan(angle, iter)*distance;
}
//Funkce vrati optimalni pocet iteraci pro zadany uhel
//Prebira jeden parametr typu double - uhel v rad
int get_iterations(double angle) {
  int iter; 
  if(angle > 0.0 && angle <= 0.2) {
      iter = 6;
    } else if(angle > 0.2 && angle <= 0.4) {
      iter = 7;
    } else if(angle > 0.4 && angle <= 0.6) {
      iter = 8;
    } else if(angle > 0.6 && angle <= 0.8) {
      iter = 9;
    } else if(angle > 0.8 && angle <= 1.0) {
      iter = 9;
    } else if(angle > 1.0 && angle <= 1.2) {
      iter = 10;
    } else if(angle > 1.2 && angle <= 1.4) {
      iter = 11;
    } else {
      angle_err();
      exit(1);
    }
 return iter;
}
//Funkce vyhodi chybove hlaseni pri prekroceni rozmezi uhlu v rad a ukonci program
void angle_err(void) {
  fprintf(stderr, "The angle have to be in range (0.0, 1.4>\n");
}
//Funkce overi, zda je pocet zadanych iteraci v povolenem rozmezi
int check_number_of_iterations(int left, int right) {
  return  (0 < left && left <= right && right < 14);
}
//Funkce vyhodi chybove hlaseni, ze pocet iteraci je mimo povoleny rozsah
void iter_num_err(void) {
  fprintf(stderr, "The iterations have to be in range 0 < N <= M < 14\n");
}
//Funkce overi, zda je zadana vyska v povolenem rozsahu
int check_height_interval(double height) {
  return (0.0 < height && height <= 100.0);
}
//Funkce vyhodi chybove hlaseni, ze vyska neni v zadanem rozsahu
void height_range_err(void) {
  fprintf(stderr,"The height have to be in range 0 < X <= 100\n");
}
