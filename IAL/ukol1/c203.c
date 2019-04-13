
/* ******************************* c203.c *********************************** */
/*  Pďż˝edmďż˝t: Algoritmy (IAL) - FIT VUT v Brnďż˝                                 */
/*  ďż˝kol: c203 - Fronta znakďż˝ v poli                                          */
/*  Referenďż˝nďż˝ implementace: Petr Pďż˝ikryl, 1994                               */
/*  Pďż˝epis do jazyka C: Vďż˝clav Topinka, zďż˝ďż˝ďż˝ 2005                             */
/*  ďż˝pravy: Kamil Jeďż˝ďż˝bek, zďż˝ďż˝ďż˝ 2018                                          */
/* ************************************************************************** */
/*
** Implementujte frontu znakďż˝ v poli. Pďż˝esnou definici typďż˝ naleznete
** v hlaviďż˝kovďż˝m souboru c203.h (ADT fronta je reprezentovďż˝na strukturou tQueue,
** kterďż˝ obsahuje pole 'arr' pro uloďż˝enďż˝ hodnot ve frontďż˝ a indexy f_index
** a b_index. Vďż˝echny implementovanďż˝ funkce musďż˝ pďż˝edpoklďż˝dat velikost pole
** QUEUE_SIZE, i kdyďż˝ ve skuteďż˝nosti jsou rozmďż˝ry statickďż˝ho pole definovďż˝ny
** MAX_QUEUE. Hodnota QUEUE_SIZE slouďż˝ďż˝ k simulaci fronty v rďż˝znďż˝ velkďż˝m poli
** a nastavuje se v testovacďż˝m skriptu c203-test.c pďż˝ed testovďż˝nďż˝m
** implementovanďż˝ch funkcďż˝. Hodnota QUEUE_SIZE mďż˝ďż˝e nabďż˝vat hodnot v rozsahu
** 1 aďż˝ MAX_QUEUE.
**
** Index f_index ukazuje vďż˝dy na prvnďż˝ prvek ve frontďż˝. Index b_index
** ukazuje na prvnďż˝ volnďż˝ prvek ve frontďż˝. Pokud je fronta prďż˝zdnďż˝, ukazujďż˝
** oba indexy na stejnďż˝ mďż˝sto. Po inicializaci ukazujďż˝ na prvnďż˝ prvek pole,
** obsahujďż˝ tedy hodnotu 0. Z uvedenďż˝ch pravidel vyplďż˝vďż˝, ďż˝e v poli je vďż˝dy
** minimďż˝lnďż˝ jeden prvek nevyuďż˝itďż˝.
**
** Pďż˝i libovolnďż˝ operaci se ďż˝ďż˝dnďż˝ z indexďż˝ (f_index i b_index) nesniďż˝uje
** vyjma pďż˝ďż˝padu, kdy index pďż˝esďż˝hne hranici QUEUE_SIZE. V tom pďż˝ďż˝padďż˝
** se "posunuje" znovu na zaďż˝ďż˝tek pole. Za tďż˝mto ďż˝ďż˝elem budete deklarovat
** pomocnou funkci NextIndex, kterďż˝ pro kruhovďż˝ pohyb pďż˝es indexy pole
** vyuďż˝ďż˝vďż˝ operaci "modulo".
**
** Implementujte nďż˝sledujďż˝cďż˝ funkce:
**
**    nextIndex ..... pomocnďż˝ funkce - viz popis vďż˝ďż˝e
**    queueInit ..... inicializace fronty
**    queueEmpty .... test na prďż˝zdnost fronty
**    queueFull ..... test, zda je fronta zaplnďż˝na (vyďż˝erpďż˝na kapacita)
**    queueFront .... pďż˝eďż˝te hodnoty prvnďż˝ho prvku z fronty
**    queueRemove ... odstranďż˝ prvnďż˝ prvek fronty
**    queueGet ...... pďż˝eďż˝te a odstranďż˝ prvnďż˝ prvek fronty
**    queueUp ....... zaďż˝azenďż˝ prvku na konec fronty
**
** Svďż˝ ďż˝eďż˝enďż˝ ďż˝ďż˝elnďż˝ komentujte!
**
** Terminologickďż˝ poznďż˝mka: Jazyk C nepouďż˝ďż˝vďż˝ pojem procedura.
** Proto zde pouďż˝ďż˝vďż˝me pojem funkce i pro operace, kterďż˝ by byly
** v algoritmickďż˝m jazyce Pascalovskďż˝ho typu implemenovďż˝ny jako
** procedury (v jazyce C procedurďż˝m odpovďż˝dajďż˝ funkce vracejďż˝cďż˝ typ void).
**
**/

#include "c203.h"

void queueError (int error_code) {
/*
** Vytiskne upozornďż˝nďż˝ na to, ďż˝e doďż˝lo k chybďż˝.
** Tato funkce bude volďż˝na z nďż˝kterďż˝ch dďż˝le implementovanďż˝ch operacďż˝.
**
** TUTO FUNKCI, PROSďż˝ME, NEUPRAVUJTE!
*/
	static const char* QERR_STRINGS[MAX_QERR+1] = {"Unknown error","Queue error: UP","Queue error: FRONT","Queue error: REMOVE","Queue error: GET","Queue error: INIT"};
	if ( error_code <= 0 || error_code > MAX_QERR )
		error_code = 0;
	printf ( "%s\n", QERR_STRINGS[error_code] );
	err_flag = 1;
}

void queueInit (tQueue* q) {
/*
** Inicializujte frontu nďż˝sledujďż˝cďż˝m zpďż˝sobem:
** - vďż˝echny hodnoty v poli q->arr nastavte na '*',
** - index na zaďż˝ďż˝tek fronty nastavte na 0,
** - index prvnďż˝ho volnďż˝ho mďż˝sta nastavte takďż˝ na 0.
**
** V pďż˝ďż˝padďż˝, ďż˝e funkce dostane jako parametr q == NULL, volejte funkci
** queueError(QERR_INIT).
*/
	if (q == NULL) {
		queueError(QERR_INIT);
		return;
	}
	// Vsechny hodnoty v poli nastavi na '*'
	for(int i = 0; i < QUEUE_SIZE; i++) {
		q->arr[i] = '*';
	}

	q->b_index = 0;
	q->f_index = 0;

	return;
}

int nextIndex (int index) {
/*
** Pomocnďż˝ funkce, kterďż˝ vracďż˝ index nďż˝sledujďż˝cďż˝ho prvku v poli.
** Funkci implementujte jako jedinďż˝ prikaz vyuďż˝ďż˝vajďż˝cďż˝ operace '%'.
** Funkci nextIndex budete vyuďż˝ďż˝vat v dalďż˝ďż˝ch implementovanďż˝ch funkcďż˝ch.
*/
	// Kruhovost pole pomoci operace modulo
	return (index+1) % QUEUE_SIZE;
}

int queueEmpty (const tQueue* q) {
/*
** Vracďż˝ nenulovou hodnotu, pokud je frona prďż˝zdnďż˝, jinak vracďż˝ hodnotu 0. 
** Funkci je vhodnďż˝ implementovat jednďż˝m pďż˝ďż˝kazem return.
*/
	// Index prvniho prvku a prvin volne pozice je shodny
	return (q->b_index == q->f_index) ? 1 : 0;
}

int queueFull (const tQueue* q) {
/*
** Vracďż˝ nenulovou hodnotu, je-li fronta plnďż˝, jinak vracďż˝ hodnotu 0. 
** Funkci je vhodnďż˝ implementovat jednďż˝m pďż˝ďż˝kazem return
** s vyuďż˝itďż˝m pomocnďż˝ funkce nextIndex.
*/
	// Zavolanim funkce nextIndex nad poslednim prvkem se dostaneme na prvni prvek
	return (q->f_index == nextIndex(q->b_index)) ? 1 : 0;
}

void queueFront (const tQueue* q, char* c) {
/*
** Prostďż˝ednictvďż˝m parametru c vrďż˝tďż˝ znak ze zaďż˝ďż˝tku fronty q.
** Pokud je fronta prďż˝zdnďż˝, oďż˝etďż˝ete to volďż˝nďż˝m funkce queueError(QERR_FRONT).
** Volďż˝nďż˝ tďż˝to funkce pďż˝i prďż˝zdnďż˝ frontďż˝ je vďż˝dy nutnďż˝ povaďż˝ovat za nekorektnďż˝.
** Bďż˝vďż˝ to totiďż˝ dďż˝sledek ďż˝patnďż˝ho nďż˝vrhu algoritmu, ve kterďż˝m je fronta
** pouďż˝ita. O takovďż˝ situaci se proto musďż˝ programďż˝tor-vďż˝vojďż˝ďż˝ dozvďż˝dďż˝t.
** V opaďż˝nďż˝m pďż˝ďż˝padďż˝ je ladďż˝nďż˝ programďż˝ obtďż˝nďż˝jďż˝ďż˝!
**
** Pďż˝i implementaci vyuďż˝ijte dďż˝ďż˝ve definovanďż˝ funkce queueEmpty.
*/
	// Fronta je prazdna
	if (queueEmpty(q) != 0) {
		queueError(QERR_FRONT);
		return;
	}

	*c = q->arr[q->f_index];
	return;
}

void queueRemove (tQueue* q) {
/*
** Odstranďż˝ znak ze zaďż˝ďż˝tku fronty q. Pokud je fronta prďż˝zdnďż˝, oďż˝etďż˝ete
** vzniklou chybu volďż˝nďż˝m funkce queueError(QERR_REMOVE).
** Hodnotu na uvolnďż˝nďż˝ pozici ve frontďż˝ nijak neoďż˝etďż˝ujte (nepďż˝episujte).
** Pďż˝i implementaci vyuďż˝ijte dďż˝ďż˝ve definovanďż˝ funkce queueEmpty a nextIndex.
*/
	// Fronta je prazdna
	if (queueEmpty(q) != 0) {
		queueError(QERR_REMOVE);
		return;
	}
	// Posune index prvniho prvku za dalsi prvek
	q->f_index = nextIndex(q->f_index);

	return;
}

void queueGet (tQueue* q, char* c) {
/*
** Odstranďż˝ znak ze zaďż˝ďż˝tku fronty a vrďż˝tďż˝ ho prostďż˝ednictvďż˝m parametru c.
** Pokud je fronta prďż˝zdnďż˝, oďż˝etďż˝ete to volďż˝nďż˝m funkce queueError(QERR_GET).
**
** Pďż˝i implementaci vyuďż˝ijte dďż˝ďż˝ve definovanďż˝ch funkcďż˝ queueEmpty,
** queueFront a queueRemove.
*/
	// Fronta je prazdna
		if (queueEmpty(q) != 0) {
			queueError(QERR_GET);
			return;
		}

		queueFront(q,c);
		queueRemove(q);

		return;
}

void queueUp (tQueue* q, char c) {
/*
** Vloďż˝ďż˝ znak c do fronty. Pokud je fronta plnďż˝, oďż˝etďż˝ete chybu volďż˝nďż˝m
** funkce queueError(QERR_UP). Vklďż˝dďż˝nďż˝ do plnďż˝ fronty se povaďż˝uje za
** nekorektnďż˝ operaci. Situace by mohla bďż˝t ďż˝eďż˝ena i tak, ďż˝e by operace
** neprovďż˝dďż˝la nic, ale v pďż˝ďż˝padďż˝ pouďż˝itďż˝ takto definovanďż˝ abstrakce by se
** obtďż˝nďż˝ odhalovaly chyby v algoritmech, kterďż˝ by abstrakci vyuďż˝ďż˝valy.
**
** Pďż˝i implementaci vyuďż˝ijte dďż˝ďż˝ve definovanďż˝ch funkcďż˝ queueFull a nextIndex.
*/

	// Fronta je prazdna
		if (queueFull(q) != 0) {
			queueError(QERR_UP);
			return;
		}

		q->arr[q->b_index] = c;
		q->b_index = nextIndex(q->b_index);
		return;
}
/* Konec pďż˝ďż˝kladu c203.c */
