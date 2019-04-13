/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<stdbool.h>
#include"errors.h"

#define TRUE 1
#define FALSE 0

typedef struct tBSTNode {
	// Nazov ID
	char *Key;

	// Pre Funkcie
	bool Declared;
	int NumberOfParametres;
	struct tBSTNode * TS;

	// Hodnota premennej
	char *Value;


	struct tBSTNode * LPtr;
	struct tBSTNode * RPtr;
} *tBSTNodePtr;


void BSTInit   (tBSTNodePtr *);
tBSTNodePtr BSTSearch  (tBSTNodePtr, char*);
void BSTInsert (tBSTNodePtr *, char*, bool, int,char*);
void BSTDelete (tBSTNodePtr *, char*);
void BSTDispose(tBSTNodePtr *, int *);
