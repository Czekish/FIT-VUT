
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

/*
** Doimplementovani metod: Daniel Stepan
*/

#include "c201.h"

int errflg;
int solved;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList(tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
    return;
}

void DisposeList(tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
    // Prvni prvek nastavi jako aktivni
    L->Act = L->First;
    // Iterujeme dokud aktivni prvek neni NULL
    while (L->Act != NULL){
        // Jako prvni prvek se dosadi prvek, na ktery ukazoval aktivni
        L->First = L->Act->ptr;
        // Uvolneni pamet aktivniho prvku
        free(L->Act);
        // Prvni prvek se nastavi jako aktivni
        L->Act = L->First;
    }
}

void InsertFirst(tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/

    // Alokace pameti pro novy prvek a kontrola na dostatek mista pro alokaci
    tElemPtr item = (tElemPtr) malloc(sizeof(struct tElem));
    if (item == NULL) {
        Error();
        return;
    }

    item->data = val;
    item->ptr = L->First;
    L->First = item;
    return;
}

void First(tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/

    L->Act = L->First;
    return;
}

void CopyFirst(tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
    // Kontrola, zda neni seznam prazdny
    if ((L->First == NULL)) {
        Error();
        return;
    } else {
        *val = L->First->data;
        return;
    }
}

void DeleteFirst(tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
    // Seznam je prazdny, nic nedelame
    if (L->First == NULL)
        return;

    // Pokud je prvni prvek zaroven aktivni, ztracime aktivitu seznamu
    if (L->First == L->Act)
        L->Act = NULL;

    // Pomocna promenna
    tElemPtr second_item;
    second_item = L->First->ptr;
    free(L->First);
    L->First = second_item;

    return;
}

void PostDelete(tList *L) {
/* 
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/

    tElemPtr item;
    // Kontrola, ze seznam je aktivni
    if (L->Act != NULL) {
        // Kontrola, ze aktivni prvek neni poslednim prvkem seznamu
        if (L->Act->ptr != NULL) {
            // Aktivni prvek ukazuje na prvek za mazanym prvkem.
            item = L->Act->ptr;
            L->Act->ptr = item->ptr;
            free(item);
        }
    }
}

void PostInsert(tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/

    // Nebyl nalezen aktivni prvek
    if (L->Act == NULL)
        return;

    // Alokace mista a vytvoreni noveho prvku
    tElemPtr item = (tElemPtr) malloc(sizeof(struct tElem));

    // Nedoslo k chybe
    if (item != NULL) {
        item->data = val;
        item->ptr = L->Act->ptr;
        L->Act->ptr = item;
    } else {
        Error();
        return;
    }

    return;
}

void Copy(tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
    // Seznam neni aktivni -> vola Error()
    if (L->Act == NULL) {
        Error();
    } else {
        *val = L->Act->data;
    }
    return;
}

void Actualize(tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/

    // Kontrola aktivity seznamu
    if (L->Act != NULL)
        L->Act->data = val;

    return;
}

void Succ(tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
    // Kontrola aktivity seznamu
    if (L->Act != NULL)
        L->Act = L->Act->ptr;
    return;
}

int Active(tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return. 
**/

    return (L->Act != NULL) ? 1 : 0;
}

/* Konec c201.c */
