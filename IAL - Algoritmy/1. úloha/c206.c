
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2019
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
    // inicicializácia na NULL-> yoynam je pr8ydnz
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;


/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    
	
// solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDisposeList (tDLList *L) {

    tDLElemPtr x = L->First;
    while(L->First != NULL ){ //  ruší vždy prvý prvok a prvý presúva na ďalší
        L->First = L->First->rptr;
        free(x);
        x = L->First;
    }
    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
    
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertFirst (tDLList *L, int val) {

    tDLElemPtr x = (tDLElemPtr)malloc(sizeof(struct tDLElem)); // alokácia pamäte
    if (!x){ // zlyhanie mallocu
        DLError();
		return;
	}
    // nastavenie hodnôt
    x->data = val;
    x->lptr = NULL;
    x->rptr = L->First;

    if (L->First == NULL){ // ak nieje v zozname žiadny prvok tak vkladanú prvok je aj last
        L->Last = x;
    } else {
        L->First->lptr = x; // lptr na prvom (už druhom) prvku ide na pridávaný prvok
    }
    // nový prvok ide na začiatok
    L->First = x;
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
    tDLElemPtr x = (tDLElemPtr)malloc(sizeof(struct tDLElem)); // alokácia pamete
    if (!x){ // zlyhanie mallocu
        DLError();
		return;
	}
    // nastavenie hodnôt
    x->data = val;
    x->lptr = L->Last;
    x->rptr = NULL;

    if (L->Last == NULL){ // ak je zoznam prázdny, tak pridávaný prvok je aj prvý
        L->First = x;
    } else {
        L->Last->rptr = x; // rptr na prvom (už druhom) prvku ide na pridávaný prvok
    }
    // nový prvok ide na koniec
    L->Last = x;
	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First; //nastavenie aktivity na prvý prvok
	

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last; //nastavenie aktivity na posledný prvok
	
	
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL){        // overenie či je prázdny
        DLError(); // ak je L prázdny volá error
    } else{
        *val = L->First->data; // nastavenie val na data prvku
    }

	
	
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/ 
	if(L->First == NULL){      // overenie či je zoznam prázdny  
        DLError(); // ak je L prázdny volá arror
    } else{
        *val = L->Last->data; //nastavenie val na data prvku
    }
	
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First == NULL){ // ka je L prázdny nič sa nedeje
        return;
    } 

    if(L->Act == L->First){ // ak bol prvý prvok aktívny activita sa ruší
        L->Act = NULL;
    }
    if(L->First == L->Last){ // ak je v zozname len jeden prvok Last ide tiež na NULL
        L->Last = NULL;
    }else {
        L->First->rptr->lptr = NULL; // ak je v zozname viac prvkov tak druhý prvok ltpr je nastavený na NULL
    }
    tDLElemPtr x = L->First;
    L->First = L->First->rptr; // First sa posúva na druhý prvok (prvok vpravo), ak prvok vpravo nieje ide na NULL
    free(x); // uvolnenie pamete

    
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if(L->First == NULL){ // ak je L prázdny nič sa nedeje
        return;
    } 

    if(L->Act == L->Last){ // ak bol posledný prvok aktívny aktivita sa stráca
        L->Act = NULL;
    }
    if(L->First == L->Last){ // ak je v zozname len jeden prvok First ide na NULL
        L->First = NULL;
    }else {
        L->Last->lptr->rptr = NULL; // ak je v zozname viac prvkov rptr predposledného ide na NULL
    }
    tDLElemPtr x = L->Last;
    L->Last = L->Last->lptr; // Last sa posúva na predposledný prvok (vľavo), ak prvok vľavo nieje ide na NULL
    free(x);

    
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/ 
    if(L->Act == L->Last){ // ak je aktívny posledný prvok nič sa nedeje
        return;
    }
    if(L->Act == NULL){ //ak nieje aktívny prvok nič sa nedeje
        return;
    }
    tDLElemPtr x = L->Act->rptr; //pomocný ukazateľ na prvok čo treba zmazať
    if(x == L->Last){ // ak je prvok čo mažeme Last
        L->Last = L->Act; //posledný prvok bude aktívnym prvkom
        L->Act->rptr = NULL; // rptr ide na NULL lebo posledný prvok sme zmazali
    } else{ //ak prvok nieje Last
        x->rptr->lptr = L->Act; // lptr prvku vpravo od prvku čo mažeme ide na aktívny prvok 
        L->Act->rptr = x->rptr; // rptr aktívneho ide na prvok naľavo od prvku čo mažeme 
    }

    free(x);
		
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if(L->Act == L->First){ //ak je aktívny prvý prvok nič sa nedeje
        return;
    }
    if(L->Act == NULL){ // ak nieje aktívny prvok
        return;
    }
    tDLElemPtr x = L->Act->lptr; //pomocný ukazateľ na prvok čo treba zmazať
    if(x == L->First){ //ak je prvok čo mažeme First
        L->First = L->Act; // prvý prvok bude aj aktívny 
        L->Act->lptr = NULL;// lptr ide na NULL lebo aktívny je prvý prvok
    } else{ // ak prvok nieje First
        x->lptr->rptr = L->Act; // rptr prvku vľavo od prvku čo mažeme ide na aktívny prvok
        L->Act->lptr = x->lptr; // lptr aktívneho ide na prvok naľavo od prvku čo mažeme
    }

    free(x);
			
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act == NULL){ // ak nieje aktívny prvok nič nerobí
        return;
    }
    tDLElemPtr x = (tDLElemPtr)malloc(sizeof(struct tDLElem)); // alkolácia pamete
    if (!x){ // ak malloc zlyhal
        DLError();
		return;
	}
    if (L->Act == L->Last){ // ak je aktívny prvok Last
        L->Last = x; // Last bude nový prvok
    } else { // ak nieje aktívny posledný prvok
        L->Act->rptr->lptr = x;  // lptr prvku vpravo od aktívneho ide na pridaný prvok
    }
    //nastavenie hodnôt prvku
    x->data = val;
    x->rptr = L->Act->rptr;
    x->lptr = L->Act;
    //rptr aktívneho ide na pridaný prvok
    L->Act->rptr = x;

	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act == NULL){ //ak nieje aktívny prvok
        return; 
    }
    tDLElemPtr x = (tDLElemPtr)malloc(sizeof(struct tDLElem)); // alokácia pamete
    if (!x){ // ak malloc zlyhal
        DLError();
		return;
	}
    if (L->Act == L->First){ //ak je aktívny First
        L->First = x; // prvým sa stane nový prvok
    } else { // ak je aktívny Last
        L->Act->lptr->rptr = x;  //rptr prvku vľavo od aktívneho ide na nový prvok
    }
    // nastavenie hodnôt
    x->data = val;
    x->lptr = L->Act->lptr;
    x->rptr = L->Act;
    //lptr aktívneho prvku ide na nový prvok
    L->Act->lptr = x;

	
// solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/     
    if(L->Act == NULL){    // ak nieje aktívny prvok
        DLError();
    } else{
        *val = L->Act->data; // nastavenie val na hodnotu aktívneho prvku
    }
		
	
	
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act == NULL){   // ak nieje aktívny prvok

    } else{
        L->Act->data = val; //prepis data v aktívnom prvku na hodnotu val
    }
	
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act == NULL){ // ak nieje aktívny prvok nič nerobí
        return;
    }
    if(L->Act == L->Last){ // ak je aktivita na poslednom prvku stráca sa
        L->Act = NULL;
        return;
    }
    // posunutie aktivity
    L->Act = L->Act->rptr;
	
    //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act == NULL){ // ak nieje aktívny prvok
        return;
    }
    if(L->Act == L->First){ //ak je aktívny prvý prvok stráca sa aktivita
        L->Act = NULL;
        return;
    }
    // posun aktivity
    L->Act = L->Act->lptr;
	
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return L->Act != NULL;
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

/* Konec c206.c*/
