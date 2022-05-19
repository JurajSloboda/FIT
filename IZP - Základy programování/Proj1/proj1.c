/**********************************************
        IZP - 1.projekt - Iterační výpočty
        Juraj Sloboda
        November 2018
***********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

double taylor_log( double x, unsigned long int n );                     //počíta logaritmus z X na N iterácií cez Taylorov polynom
double cfrac_log( double x, unsigned long int n );                      //počíta logaritmus z X na N iterácií cez zreťazený zlomok
double taylor_pow( double x, double y, unsigned long int n );           //počíta pow(X,Y) na N iterácií za použitia taylor_log
double taylorcf_pow( double x, double y, unsigned long int n );         //počíta pow(X,Y) na N iterácií za použitia taylorcf_log
double get_pow( double x, double y, unsigned long int n , int o);       //pomocná funkcia na výpočet pow
double get_double(char str[], char *meno, bool *chyba);                 //získava argv a zisťuje či zadaný argv bol typu double a vracia jeho hodnotu
long int get_int(char str[], bool *chyba);                              //získava argv a zisťuje či zadaný argv bol typu integer a vracia jeho hodnotu
double my_log(double x);                                                //počíta log pokiaľ nedosiahne určitú presnosť
double special_log(double x);                                           //špeciálne hodnoty pre logaritmus
double my_pow(double x, double y);                                      //počíta pow pokiaľ nedosiahne určitú presnosť

int main( int argc, char *argv[] ){

        double x, y;
        unsigned long int n;
        bool chyba;

        if ( argc == 4 && strcmp( "--log", argv[ 1 ] ) == 0 ){                  //vykoná sa pri zadanom --log X N
                chyba = false;
                //získanie hodnôt X a N a overenie či argv boli daných typov
                x = get_double(argv[2], "X", &chyba);
                n = get_int(argv[3], &chyba);
                //výpis výsledkov ak nenastala chyba pri načítaní hodnôt
                if (!chyba){
                        fprintf( stdout, "       log(%g) = %.12g\n", x, log( x ) );
                        fprintf( stdout, " cfrac_log(%g) = %.12g\n", x, cfrac_log( x, n ) );
                        fprintf( stdout, "taylor_log(%g) = %.12g\n", x, taylor_log( x, n ) );
                        return 0;
                }
        } else if ( argc == 5 && strcmp( "--pow", argv[ 1 ] ) == 0 ){            //vykoná sa pri zadanom --pow X Y N
                chyba = false;
                //získanie hodnôt X, Y a N a overenie či argv boli daných typov
                x = get_double(argv[2], "X", &chyba);
                y = get_double(argv[3], "Y", &chyba);
                n = get_int(argv[4], &chyba);
                //funkcia pow je len pre kladné čísla X väčšie ako 0
                if (x<=0){
                        fprintf(stderr,"Error: x musi byt kladne cislo vacsie ako 0\n");
                        return 1;
                }
                //výpis výsledkov ak nenastala chyba pri načítaní hodnôt
                if (!chyba){
                        fprintf( stdout, "         pow(%g,%g) = %.12g\n", x, y, pow( x, y ) );
                        fprintf( stdout, "  taylor_pow(%g,%g) = %.12g\n", x, y, taylor_pow( x, y, n ) );
                        fprintf( stdout, "taylorcf_pow(%g,%g) = %.12g\n", x, y, taylorcf_pow( x, y, n ) );
                        return 0;
                }
        } else if ( argc == 3 && strcmp( "--mylog", argv[ 1 ] ) == 0 ){         //vykoná sa pri zadanom --mylog X
                chyba = false;
                //získanie hodnoty X a overenie či argv bol daného typu
                x = get_double(argv[2], "X", &chyba);
                //výpis výsledkov ak nenastala chyba pri načítaní hodnoty
                if (!chyba){
                        fprintf( stdout, "  log(%g) = %.7e\n", x, log( x ) );
                        fprintf( stdout, "mylog(%g) = %.7e\n", x, my_log(x));
                        return 0;
                } 
        } else if ( argc == 4 && strcmp( "--mypow", argv[ 1 ] ) == 0 ){         //vykoná sa pri zadanom --mypow X Y
                chyba = false;
                //získanie hodnôt X a Y a overenie či argv boli daných typov
                x = get_double(argv[2], "X", &chyba);
                y = get_double(argv[3], "Y", &chyba);
                //funkcia pow je len pre kladné čísla X väčšie ako 0
                if (x<=0){
                        fprintf(stderr,"Error: x musi byt kladne cislo vacsie ako 0\n");
                        return 1;
                }
                //výpis výsledkov ak nenastala chyba pri načítaní hodnôt
                if (!chyba){
                        fprintf( stdout, "   pow(%g,%g) = %.7e\n", x, y, pow( x, y ) );
                        fprintf( stdout, "my_pow(%g,%g) = %.7e\n", x, y, my_pow( x, y ) );
                        return 0;
                }

        } else{                                 //ak boli zadané nesprávne argumenty
                fprintf( stderr, "Error: nesprávne argumenty, zadaj:\n--log X N\n--pow X Y N\n--mylog X\n--mypow X Y\n" );
                return 1;
        }
}

//get_double: získava argv a zisťuje či zadaný argv bol typu double, ak áno vracia jeho hodnotu, ak nie mení chyba na true
double get_double(char str[], char *meno, bool *chyba){
        char *string;
        double d;
        d = strtod(str, &string);
        //zistenie či argv bol typu double
        if (strlen(string) != 0){
                fprintf(stderr, "Error: %c musi byt typu double \n", *meno);
                *chyba = true;
        }
        return d;
}

//get_int: získava argv a zisťuje či zadaný argv bol typu integer, ak áno vracia jeho hodnotu, ak nie mení chyba na true
long int get_int(char str[], bool *chyba){
        char *string;
        long int n;
        n = strtol(str, &string, 0);
        //zistenie či argv bol typu double
        if (strlen(string)!= 0){
                fprintf(stderr, "Error: N musi byt typu integer\n");
                *chyba = true;
        }
        //počet iterácií musí byť celé číslo, minimálne 1
        if ((long int)n <= 0){
                fprintf(stderr, "Error: N musi byt kladne cele cislo minimalne 1\n");
                *chyba = true;
        }
        return n;
}

//special_log: overuje niektoré špeciálne hodnoty logaritmu
double special_log(double x){
        if (x < 0){
                //keď je x záporné tak log(x) je NAN
                return NAN;
        } else if ( x == 0){
                //keď je x rovné 0 tak log(x) je -inf
                return -INFINITY;
        } else if ( x == 1) {
                //log(1) = 0
                return 0;
        } else if ( isnan(x) ){
                //keď je x NAN tak výsledok je tiež NAN
                return NAN;
        } else if ( isinf(x) ){
                //ked je x inf log(x) je tiez inf
                return INFINITY;
        }
        return 9;
}

//taylor_log: počíta logaritmus z X na N iterácií cez Taylorov polynom
double taylor_log( double x, unsigned long int n ){
        //overenie špeciálnych hodnôt ak nejaká nastane dáva hneď výsledok
        double special = special_log(x);
        if (special != 9){
                return special;
        }
        double result, y, y_new, x_new;
        unsigned long int i;
        result = 0;
        //použitie vzorca pre interval 0 < x <= 1 
        if ( (double) x > 0 && (double) x <= 1 ){
                y = 1 - x;
                y_new = y;
                //cyklus na vykonanie N iterácií
                for( i = 1; (unsigned long int) i <= (unsigned long int) n; i++ ){
                        result -= ( y_new / i ); 
                        y_new *= y;
                }            
                
        }
        //použitie vzorca pre interval 1 < x
        if ( (double) x > 1 ){
                x_new = ( x - 1 ) / x;
                //cyklus na vykonanie N iterácií
                for ( i = 1; (unsigned long int) i <= (unsigned long int ) n; i++ ){
                        result += x_new / i;
                        x_new *= ( x - 1 ) / x;
                }
        }
        
        return result;
}

//cfrac_log: počíta logaritmus z X na N iterácií cez zreťazený zlomok
double cfrac_log( double x, unsigned long int n ){
        //overenie špeciálnych hodnôt ak nejaká nastane dáva hneď výsledok
        double special = special_log(x);
        if (special != 9){
                return special;
        }

        double z , z_last, z_new = 0;
        unsigned long int i;
        //výpočet z, ktoré sa využíva vo vzorci
        z = ( x - 1 ) / ( x + 1 );
        z_last = 0;
        //cyklus pre N iterácií
        for ( i = n; (unsigned long) i >= 2; i-- ){
                z_new = ((i-1)*(i-1)*z*z)/((2*i - 1) - z_last);
                z_last = z_new;
        }
        z_new = ( 2 * z ) / (1 - z_last);
        return z_new;
}

//taylor_log: počíta pow(X,Y) na N iterácií za použitia taylor_log
double taylor_pow( double x, double y, unsigned long int n ){
        //volá funkciu get_pow ktorá vypočíta pow
        return get_pow( x, y, n, 0 );
}

//taylorcf_pow: počíta pow(X,Y) na N iterácií za použitia taylorcf_log
double taylorcf_pow( double x, double y, unsigned long int n ){
        //volá funkciu get_pow ktorá vypočíta pow
        return get_pow( x, y, n, 1 );
}

//get_pow: pomocná funkcia na výpočet pow - logaritmus berie podľa argumentu o
double get_pow( double x, double y, unsigned long int n , int o ){
        //overenie špeciálnych vstupov
        if (x == 1 || y == 0){
                //v týchto prípadoch je výsledok vždy 1
                return 1;
        } else if ( isnan(x) || isnan(y) ){
                // ak je X alebo Y NAN tak výsledok je tiež NAN
                return NAN;
        } else if ( isinf(x) || isinf(y) ){
                // ak je X alebo Y inf tak výsledok je tiež inf
                return INFINITY;
        }
        double result, x_last, logaritmus;
        unsigned long int i;
        result = 1;
        x_last = 1;
        //zistenie či má zobrať log z cfrag_log alebo taylor_log podla toho či o je rovné 1 alebo 0
        if ( o == 1 ){
                logaritmus = cfrac_log( x, n );
        }
        else{ 
                logaritmus = taylor_log( x, n );
        }
        //for cyklus na výpočet N iterácií
        for( i = 1; (unsigned long int) i <= (unsigned long int) (n - 1); i++ ){
                x_last *= ( y * logaritmus ) / i;
                result += x_last;
        }
        
        return result;
}

//my_log: počíta log pokiaľ nedosiahne určitú presnosť
double my_log(double x){
        //overenie špeciálnych hodnôt ak nastane niektorá znich tak automaticky vracia výsledok
        double special = special_log(x);
        if (special != 9){
                return special;
        }
        double lastCfrac = 0.0, Cfrac = 0.0, Taylor = 0.0, lastTaylor = 0.0, result = 0.0, xTaylor = 1.0;
        bool hotovo = false;
        unsigned long int i;
        // for cyklus ktorý beží pokial nenastane stav hotovo = true
        for (i = 1; !hotovo; i++){
                //počítanie logaritmu pomocou Taylorovho polynomu
                lastTaylor = Taylor;
                if ( (double) x > 1 ){
                        xTaylor *= ( x - 1 ) / x;
                        Taylor += xTaylor / i;
                }
                if ( (double) x > 0 && x <= 1){
                        xTaylor *= (1-x);
                        Taylor -= xTaylor / i; 
                }
                //počítanie logaritmu pomocou zreťazeného zlomku
                lastCfrac = Cfrac;
                Cfrac = cfrac_log(x, i);
                //overenie či Taylorov polynom nedosiahol danú presnosť
                if ( (fabs (Taylor - lastTaylor)) <= 1e-8 ){
                        result = Taylor;
                        hotovo = true;
                }
                //overenie či zreťazený zlomok nedosiahol danú presnosť
                if ( (fabs (Cfrac - lastCfrac)) <= 1e-8){
                        //ak presnosť dosiahli oba logaritmy naraz v tej istej iterácii porovná ich a do result uloží ten presnejší
                        if (!hotovo || (fabs (Cfrac - lastCfrac)) < (fabs (Taylor - lastTaylor))){
                                result = Cfrac;
                                hotovo = true;
                        }
                }
        }
        //printf("%ld\n",i);
        return result;
}

//my_pow: počíta pow pokiaľ nedosiahne určitú presnosť
double my_pow(double x, double y){
        //overenie špeciálnych vstupov
        if (x == 1 || y == 0){
                //v týchto prípadoch je výsledok vždy 1
                return 1;
        } else if ( isnan(x) || isnan(y) ){
                // ak je X alebo Y NAN tak výsledok je tiež NAN
                return NAN;
        } else if ( isinf(x) || isinf(y) ){
                // ak je X alebo Y inf tak výsledok je tiež inf
                return INFINITY;
        }

        double logaritmus = 0, lastMypow = 0 , Mypow = 1, xMypow = 1;
        unsigned long int i = 1;
        //berie logaritmus z my_log
        logaritmus = my_log(x);
        //vykonávanie cyklu pokiaľ nedosiahne určitú presnosť
        do{
                lastMypow = Mypow;
                xMypow *= ( y * logaritmus ) / i;
                Mypow += xMypow;
                i++;
        } while (fabs(Mypow - lastMypow) >= 1e-8 ); //overenie presnosti, ak bola dosiahnutá cyklus končí
        //fprintf( stdout, "%ld\n",i);
        return Mypow;
}
