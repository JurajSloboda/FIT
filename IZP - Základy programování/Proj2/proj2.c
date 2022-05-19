/*****************************************************
        IZP - 3.projekt - Jednoduchá shluková analýza
        Juraj Sloboda
        December 2018
******************************************************
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    if (cap > 0){
        //alokovanie miesta pre objekt
        c->obj = (struct obj_t*) malloc( sizeof(struct obj_t)*cap );
        //printf("alloc");
        if (c->obj == NULL){
            c->capacity = 0;
        }
    } else if (cap == 0){
        c->obj = NULL;
    }
    //nastavenie kapacity a veľkosti
    c->size = 0;
    c->capacity = cap;

}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{   
    //uvoľnenie pamäti a znovu inicializacia 
    free(c->obj);
    init_cluster(c,0);

}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{   
    //rozšírenie clustera ak je potreba
    if (c->capacity == c->size){
        c = resize_cluster(c, c->capacity + 1);
    }
    
    //pridanie objektu do clustera
    c->obj[c->size].id = obj.id;
    c->obj[c->size].x = obj.x;
    c->obj[c->size].y = obj.y;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    //pridanie objektov z clustera c2 do clustera c1
    int i;
    for(i = 0; i< c2->size; i++){
        append_cluster(c1, c2->obj[i]);
    }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    
    //odstránenie clustera
    clear_cluster(&carr[idx]);
    int i;
    //posunutie zvyšných clusterov
    for (i=idx; i<narr - 1;i++){
        carr[i] = carr[i+1];
    }
    
    return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    //výpočet vzdialenosti
    return sqrtf((o1->x-o2->x)*(o1->x-o2->x)+(o1->y-o2->y)*(o1->y-o2->y));

}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);


    //výpočet minimálne vzdialenosti objektov v clusteroch
    long int i,j;
    float dis = 10000, pdis = 0;
    for (i=0;i<c1->size;i++){
        for(j=0;j<c2->size;j++){
            pdis = obj_distance(&c1->obj[i],&c2->obj[j]);
            if(pdis < dis){
                dis = pdis; 
            }
        }
    }

    return dis;

}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    int i,j;
    float dis = 0, pdis = 0;

    
    dis = cluster_distance(&carr[0],&carr[1]);
    *c1 = 0;
    *c2 = 1;
    for (i=0; i<narr; i++){
        for (j= i+1; j<narr; j++){
            pdis = cluster_distance(&carr[i],&carr[j]);
            if (pdis<dis){
                dis = pdis;
                *c1 = i;
                *c2 = j;
            }

        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    
    int count = 0;
    int i;
    int j;
    char str[50];
    char* str_txt;
    char* str_num;

    //otvorenie súboru
    FILE* fr;
    fr = fopen(filename,"r");
    if (fr == NULL){
        *arr= NULL;
        return -1;
    }

    fgets(str,50,fr);
    str_txt = strtok(str,"=");
    str_num = strtok(NULL,"\0");


    if (strcmp("count",str_txt)!=0){
        *arr = NULL;
        return -1;
    }

    //získanie count
    char *string;
    count = strtol(str_num, &string, 0);
    //zistenie či count je typu int
    if (strlen(string)!=2){
        fprintf(stderr, "Error: count musi byt typu integer\n");
        *arr = NULL;
        return -1;
        
    }

    //alokácia miesta pre clustery
    *arr = (struct cluster_t*) malloc(sizeof(struct cluster_t)*count);
    //printf("alloc");
    if(*arr == NULL){
        return -1;
    }

    long int id;
    float x,y;
    struct obj_t object;

    //načítavanie objektov a priradenie každého do jedného clusteru
    for (i=0;i<count; i++){
        if(fgets(str,50,fr)== NULL){
            fclose(fr);
            fprintf(stderr, "Error: v subore je menej objektov ako bolo uvedene\n");
            return i;
        }

        //overenie vstupu
        for (unsigned int j = 0; j < strlen(str); j++) {
            if (!(isdigit(str[j]) || isspace(str[j]))) {
                fprintf(stderr, "Error: nespravne objekty\n");
                fclose(fr);
                return i;
            }
        }
        if (sscanf(str, "%d %f %f", &id, &x, &y) != 3) {
            fprintf(stderr, "Error: nespravne objekty\n");
            fclose(fr);
            return i;
        }

        //overenie rozsahu X a Y
        if ( (x<0) || (x>1000) || (y<0) || (y>1000) ){
            fprintf(stderr,"Error: objekt je mimo pola");
            fclose(fr);
            return i;
        }
        object.id = id;
        object.x = x;
        object.y = y;

        //inicializácia clusteru a pridanie objektu do daného clusteru
        init_cluster(*arr+i,1);
        //printf("alloc %d\n",i);
        append_cluster(*arr+i,object);

    }

    fclose(fr);
    return i;
    // TODO
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    //overenie argumentov
    if ((argc > 3) || (argc < 2)){
        fprintf(stderr,"Error: nespravne spustenie");
        return -1;
    }

    //získanie N
    char* str;
    long int n;
    if (argc == 2){
        n=1;
    }else{
        n=strtol(argv[2], &str, 0);
        if (strlen(str)!=0){
            fprintf(stderr,"Error: nespravny druhy arguent");
            return -1;
        }
    }


    int count;

    //načítanie objektov (každý do jedného clusteru)
    count = load_clusters(argv[1], &clusters);
    if (n<=count){
        //pospájanie clusterov do toľkých koľko je zadané
        int c1 = 0,c2 = 0;
        while (count > n) {
            find_neighbours(clusters, count, &c1, &c2);
            merge_clusters(&clusters[c1], &clusters[c2]);
            count = remove_cluster(clusters, count, c2);
        }
    
        //výpis clusterov
        print_clusters(clusters,count);
    } else {
        fprintf(stderr,"Error: zadanych bolo  viac clusterov ako objektov\n");
    }


    //odstránenie clusterov - uvoľnenie pamäti 
    while (count != 0) {
      count = remove_cluster(clusters, count, count-1); 
    } 

    free(clusters);

    return 0;
 
}