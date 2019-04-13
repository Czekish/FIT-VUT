/**
 * Implementace prekladace jazyka IFJ18
 *
 * xstepa60     Daniel Stepan
 * xcalad01     Filip Caladi
 * xtimko02     Jakub Timko
 * xfasan00     Erika Fasankova
 *
 */


// TODO kompatibilita s h suborom.
#include <string.h>
#include "symtable.h"
int solved;
/** Inicializacia stromu */
void BSTInit (tBSTNodePtr *RootPtr) {

	(*RootPtr) = NULL;

}
/** Vyhladavanie polozky stromu na zaklade kluca a nasledne
		vratenie ukazatela na dany polozku.
 */
tBSTNodePtr BSTSearch (tBSTNodePtr RootPtr, char* K){

	if(RootPtr == NULL || K == NULL)
	 	return NULL;

	else if(!strcmp(RootPtr->Key,K))
		return RootPtr;

	else if(strcmp(RootPtr->Key,K) < 0)
	 		BSTSearch(RootPtr->RPtr,K);

	else if(strcmp(RootPtr->Key,K) > 0)
			BSTSearch(RootPtr->LPtr,K);
	else
		return NULL;
}

/** Vkladanie do stromu */
void BSTInsert (tBSTNodePtr* RootPtr, char* K, bool Content,int NumberOfParams,char* V)	{


	if((*RootPtr) == NULL){

		(*RootPtr) = (tBSTNodePtr)malloc(sizeof(struct tBSTNode));
		if(*RootPtr == NULL)
			return;

		(*RootPtr)->TS = NULL;
		(*RootPtr)->Key = NULL;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->RPtr = NULL;

		size_t len = strlen(K);
		(*RootPtr)->Key = (char*)malloc(( (int) len + 1) * sizeof(char));
		if((*RootPtr)->Key == NULL){
			free(*RootPtr);
			return;
		}

		strcpy((*RootPtr)->Key,K);
		(*RootPtr)->NumberOfParametres = NumberOfParams;
		(*RootPtr)->Declared = Content;

	}
	else if(strcmp((*RootPtr)->Key,K) < 0)
		BSTInsert(&(*RootPtr)->RPtr,K,Content,NumberOfParams,V);

	else if(strcmp((*RootPtr)->Key,K) > 0)
		BSTInsert(&(*RootPtr)->LPtr,K,Content,NumberOfParams,V);

	else if(!strcmp((*RootPtr)->Key,K))
		(*RootPtr)->Declared = Content;

}
/**Pomocna funkcia vyuzivana pri mazani polozky stromu.
 * Jej ulohou je vyhladat najpravejsi uzol daneho stromu.
 */
void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {


	 if((*RootPtr)->RPtr != NULL){
		 ReplaceByRightmost(PtrReplaced,&(*RootPtr)->RPtr);
	 }
	 else{

		 strcpy(PtrReplaced->Key,(*RootPtr)->Key);
		 PtrReplaced->Declared = (*RootPtr)->Declared;

		 if((*RootPtr)->LPtr != NULL){

				tBSTNodePtr pomNode = (*RootPtr)->LPtr;
				free(*RootPtr);
				*RootPtr = NULL;
				(*RootPtr) = pomNode;

		 }
		 else{

			free(*RootPtr);
			*RootPtr = NULL;

		}

	 }
 }
/** Mazanie polozky danej klucom K zo stromu s vyuzitim funkcie ReplaceByRightmost  */
void BSTDelete (tBSTNodePtr *RootPtr, char* K) 		{


	if((*RootPtr) == NULL)
		return;

	else if(strcmp((*RootPtr)->Key,K) < 0)
		BSTDelete(&(*RootPtr)->RPtr,K);

	else if(strcmp((*RootPtr)->Key,K) > 0)
		BSTDelete(&(*RootPtr)->LPtr,K);

	else if(!strcmp((*RootPtr)->Key,K)){

		tBSTNodePtr pomNode = NULL;

		if ((*RootPtr)->RPtr == NULL && (*RootPtr)->LPtr == NULL) {
			free(*RootPtr);
			*RootPtr = NULL;
		}
		else if((*RootPtr)->RPtr == NULL){

			pomNode = (*RootPtr)->LPtr;
			free((*RootPtr));
			(*RootPtr) = pomNode;


		}
		else if((*RootPtr)->LPtr == NULL){

			pomNode = (*RootPtr)->RPtr;
			free((*RootPtr));
			(*RootPtr) = pomNode;

		}
		else{

			ReplaceByRightmost(*RootPtr,&(*RootPtr)->LPtr);
		}
	}
}

/** Zrusenie celeho stromu */
void BSTDispose (tBSTNodePtr *RootPtr,int *err) {

	if(*RootPtr == NULL)
		return ;

	if ((*RootPtr)->LPtr != NULL){
		BSTDispose(&(*RootPtr)->LPtr,&(*err));
	}
	if ((*RootPtr)->RPtr != NULL){
		BSTDispose(&(*RootPtr)->RPtr,&(*err));
	}
	if((*RootPtr)->TS != NULL){
		BSTDispose(&(*RootPtr)->TS,&(*err));
	}
	if((*RootPtr)->Key != NULL)
		free((*RootPtr)->Key);

	if((*RootPtr)->Declared == false)
		*err = ERR_SEM;

	free(*RootPtr);
	(*RootPtr) = NULL;
}
