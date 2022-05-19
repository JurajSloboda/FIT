/*********************************************
*IOS project 1
*Juraj Sloboda xslobo07
*April 2019
*********************************************/
// include 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <ctype.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

//proces serf a hacker - generujú procesy serfs a hackers
void Serf(int MembersCount, int Time, FILE *out, int Capacity, int BackTime, int TravelTime);
void Hacker(int MembersCount, int Time, FILE *out, int Capacity, int BackTime, int TravelTime);
// funkcia pre každého člena, category 0 pre hackers a 1 pre serfs
void MemberProcess( int category, int i, FILE *out, int Capacity, int BackTime, int TravelTime);
// funkcia na získanie pozície na lodi - captain(+ typ kapitána) alebo serf
void GetBoatPosition(int *position);
//konštruktor a deštruktor pre semafory a zdielanú pamäť
int createall();
void killall();


// zdielaná pamäť
int *Counter, // počíta všetky výpisy do out file
    *HackersCount, // počet hackers na mole
    *SerfsCount,   // počet serfs na mole
    *HackersCountPassed, // počet hackers ktorý získali pozíciu na lodi
    *SerfsCountPassed; // počet serfs ktorý získali pozíciu na lodi
    

int shmcounter,
    hackcount,
    serfcount,
    hackcountpassed,
    serfcountpassed;

//semafory potrebné na usmernenie
sem_t 
        *SemLog, //semafor na zápis do súboru out
        *SemHacker, // semafor na "lístok" na loď pre hackera 
        *SemSerf,   // semafor na "lístok" na loď pre serf 
        *SemPass,   // semafor na pozíciu
        *SemCaptainExit, // semafor pre čakanie captaina kým sa vylodia ostatný
        *SemMemberExit; // semafor pre members kým skončí plavba


FILE *out; // súbor na výstup

int main(int argc, char *argv[]){

    signal(SIGTERM, killall); 
    signal(SIGINT, killall); 
    signal(SIGSEGV, killall);


    // pid pre proces hack a serf
    pid_t HackPid,
          SerfPid;
    
   
    //overenie argumentov
    if (argc != 7){
        fprintf(stdout,"je potrebných 6 argumentov\n");
        return 1;
    }
    int MembersCount, // počet hackers a serfs
        HackersTime, // maximálny čas pre generovanie hackers
        SerfsTime, // maximálny čas pre generovanie serfsf
        TravelTime, // maximálny čas pre plavbu
        BackTime, // maximálny čas pre návrat na molo
        PierCapacity; // kapacita mola
    // err ak nebol správne zadaný vstup, napr to nieje celé číslo
    char *err;
    // chyba ak čísla nezodpovedajú potrebným číslam alebo nastal err
    bool chyba = false;

    MembersCount = strtoul(argv[1],&err,10);
    if (*err != '\0'){
        chyba = true;        
    }
    if ( !(MembersCount >= 2) || ((MembersCount % 2) != 0) ){
        chyba = true;
    }
    
    HackersTime = strtoul(argv[2],&err,10);
    if (*err != '\0'){
        chyba = true;
    }
    if ( !((HackersTime >= 0) && (HackersTime <= 2000 )) ){
        chyba = true;
    }

    SerfsTime = strtoul(argv[3],&err,10);
    if (*err != '\0'){
        chyba = true;
    }
    if ( !((SerfsTime >= 0) && (SerfsTime <= 2000 )) ){
        chyba = true;
    }

    TravelTime = strtoul(argv[4],&err,10);
    if (*err != '\0'){
        chyba = true;
    }
    if ( !((TravelTime >= 0) && (TravelTime <= 2000 )) ){
        chyba = true;
    }

    BackTime = strtoul(argv[5],&err,10);
    if (*err != '\0'){
        chyba = true;
    }
    if ( !((BackTime >= 20) && (BackTime <= 2000 )) ){
        chyba = true;
    }

    PierCapacity = strtoul(argv[6],&err,10);
    if (*err != '\0'){
        chyba = true;
    }
    if ( !(PierCapacity >= 5)  ){
        chyba = true;
    }
    // ak nastala chyba (vstupy nezodpovedajú v niečom potrebným vstupom, výpis nápovedy a ukončenie programu)
    if (chyba){

        fprintf(stdout, "Potrebné je zadať 6 argumentov: \n ");
        fprintf(stdout, "P = počet pasažierov každej kategorie (P serfs a P hackers) \n");
        fprintf(stdout, "P >= 2 && (P %% 2) = 0 \n");
        fprintf(stdout, "H = maximálna doba po ktorú je generovaný proces hackers \n");
        fprintf(stdout, "H >= 0 && H <= 2000 \n");
        fprintf(stdout, "S = maximálna doba po ktorú je generovaný proces serfs \n");
        fprintf(stdout, "S >= 0 && S <= 2000 \n");
        fprintf(stdout, "R = maximálna doba plavby \n" );
        fprintf(stdout, "R >= 0 && R <= 2000 \n");
        fprintf(stdout, "W = maximálna doba po ktorú sa osoba vracia na molo \n" );
        fprintf(stdout, "W >= 20 && W <= 2000 \n");
        fprintf(stdout, "C = kapacita mola \n");
        fprintf(stdout, "H >= 5\n");
        fprintf(stdout, "Všetky parametre sú celé čísla \n");   
        return 1;     
    }


    // volanie konštruktoru pre semafory a zdielanú pamäť, ak vnom nastala chyba výpis na stderr, volanie deštruktoru a ukončenie programu 
    if (createall() != 0){
        killall();
        return 1;
    }
    // otvorenie súboru na zápis výstupu
    if ((out = fopen("proj2.out", "w+")) == NULL)
    {                        
        fprintf(stdout, "file open error");
        killall();
        return 1;
    }
    
    // vytvorenie HackPid 
    if ((HackPid = fork()) < 0) {
        // chyba pri fork-> volanie deštruktoru a ukončenie programu
        fprintf(stderr,"hack fork error"); 
        killall();
        return 1;
    }
    if (HackPid == 0) { // child
        // volanie fukńkcie hacker ktorá vytvára hack members 
        Hacker(MembersCount, HackersTime,out, PierCapacity,BackTime, TravelTime);
    } 
    
    if ((SerfPid = fork()) < 0) {
        // chyba pri fork-> volanie deštruktoru a ukončenie programu
        fprintf(stderr,"serf fork error"); 
        killall();
        return 1;
    }
    if (SerfPid == 0) { // child
        // volanie fukńkcie serf ktorá vytvára serf members 
        Serf(MembersCount, SerfsTime,out, PierCapacity,BackTime, TravelTime);
    } 

    
    // čakanie na ukončenie procesov
    waitpid(HackPid, NULL, 0);
    waitpid(SerfPid, NULL, 0);

    // uzavretie všetkých súborov, semaforov a zdielanej pamäti
    killall();
    fclose(out);
    exit(0);

}

// funkcia pre vytvorenie procesov serfs
// MembersCount = počet vytváraných procesov
// Time = maximálny čas pre vytvorenie procesu
// *out = súbor na zápis výstupov
// capacity = kapacita mola
// BackTime = maximálny čas návratu na molo
// TravelTime = maximálny čas plavby
void Serf(int MembersCount, int Time, FILE *out, int Capacity, int BackTime, int TravelTime){
    // index procesu serf
    int i = 1;
    pid_t SerfPid;

    // vytvorenie serf members
    while( i<= MembersCount){
        // ak čas nieje 0 tak uspanie na random čas od 0 po Time
        if (Time != 0){
            usleep(random()%Time);
        }

        if ((SerfPid = fork()) < 0) {
            fprintf(stderr, "serf member fork error"); 
            exit(2);
        }
        if (SerfPid == 0) { // child
            // vytvorenie member procesu
            MemberProcess(1,i,out, Capacity,BackTime, TravelTime);
        } 
        i++;
    }

    // čakanie na ukončenie všetkých vytvorených procesov
    waitpid(MembersCount, NULL, 0); 

    exit(0);
}

// funkcia pre vytvorenie procesov hackers
// MembersCount = počet vytváraných procesov
// Time = maximálny čas pre vytvorenie procesu
// *out = súbor na zápis výstupov
// capacity = kapacita mola
// BackTime = maximálny čas návratu na molo
// TravelTime = maximálny čas plavby
void Hacker(int MembersCount, int Time, FILE *out, int Capacity, int BackTime, int TravelTime){
    // index procesu hacker
    int i = 1;
    pid_t HackerPid;
    // vytvorenie hack members
    while( i<= MembersCount){
        // ak čas nieje 0 tak uspanie na random čas od 0 po Time
        if (Time != 0){
            usleep(random()%Time);
        }
        if ((HackerPid = fork()) < 0) {
            fprintf(stderr,"hack member fork error"); 
            exit(2);
        }
        if (HackerPid == 0) { // child
            // vytvorenie member procesu
            MemberProcess(0,i,out, Capacity,BackTime, TravelTime);
        }  


        i++;
    }

    // čakanie na ukončenie všetkých vytvorených procesov
    waitpid(MembersCount, NULL, 0); 
    exit(0);

}

// funkcia pre members
// catagory- 0=hacker, 1 = serf
// i = index člena
// *out = súbor na zápis výstupov
// capacity = kapacita mola
// BackTime = maximálny čas návratu na molo
// TravelTime = maximálny čas plavby
void MemberProcess( int category, int i, FILE *out , int Capacity, int BackTime, int TravelTime)
{   

    // zápis do súboru po vytvorení procesu
    sem_wait(SemLog);     
    *Counter = *Counter + 1;
    // zápis podla kategorie
    if (category == 0){
        fprintf(out, "%d  : HACK %d  : starts\n",*Counter,i);
    }else{
        fprintf(out, "%d  : SERF %d  : starts\n",*Counter,i);
    }
    fflush(out);
    sem_post(SemLog);

    
    bool pier = false, // false member ešte nieje na mole, true ak je na mole
         back = false; // premenná pre zistenie či sa member už pokúsil dostať na molo

    // while cyklus až pokial sa member nedostane na molo
    while (!pier){
        sem_wait(SemLog); 
        // ak sa member už pokúsil dostať na molo tak výpis návratu
        if (back){
            
            *Counter = *Counter + 1;
            if (category == 0){
                fprintf(out, "%d  : HACK %d  : is back\n", *Counter, i);
            }else{
                fprintf(out, "%d  : SERF %d  : is back\n", *Counter, i);
            }
            fflush(out);
        }

        // ak je na mole miesto tak zmena pier na true(member už je na mole), a vypis že member čaká na mole
        if ((*HackersCount + *SerfsCount) < Capacity ){
           
            *Counter = *Counter + 1;
            // overenie či je serf alebo hacker
            if (category == 0){
                // pripočítanie do počtu hackerov
                *HackersCount = *HackersCount + 1;
                fprintf(out, "%d  : HACK %d  : waits               : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
            }else{
                // pripočítanie do počtu serfs
                *SerfsCount = *SerfsCount + 1;
                fprintf(out, "%d  : SERF %d  : waits               : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
            }
            fflush(out);
            sem_post(SemLog);
            pier = true;
        } else if ((*HackersCount + *SerfsCount) >= Capacity ){ // ak na mole nieje miesto
            
            *Counter = *Counter + 1;
            //overenie serf alebo hacker a výpis že opustil molo
            if (category == 0){
                fprintf(out, "%d  : HACK %d  : leaves queue        : %d      : %d\n",*Counter,i, *HackersCount, *SerfsCount);
            }else{
                fprintf(out, "%d  : SERF %d  : leaves queue        : %d      : %d\n",*Counter,i, *HackersCount, *SerfsCount);
            }
            fflush(out);
            sem_post(SemLog);
            // uspanie na náhodný čas medzi 20 až BackTime
            BackTime = BackTime - 20;
            if (BackTime == 0){
                usleep(20);
            }else{
                usleep(random()%BackTime + 20 );
            }
            // proces sa pokusil dostať na molo
            back = true;
        }

    }


    // čakanie na pass 
    sem_wait(SemPass);
    
    // pripočítanie do počtu passed (podla kategorie)
    if (category == 0){
        *HackersCountPassed = *HackersCountPassed + 1;
    }else{
        *SerfsCountPassed = *SerfsCountPassed + 1;
    }


    int position; // premenná na určenie pozície na lodi
    // získanie pozície na lodi
    // 0 = member
    // 1 = kapitán hackers lodi
    // 2 = kapitán serfs lodi   
    // 3 = kapitán zmiešanej lodi
    GetBoatPosition(&position);
    
    // ak je member tak čaka na lístok od kapitána 
    // čaká na taký do akej kategorie patrí
    if (position == 0){
        if (category == 0){
            sem_wait(SemHacker);           
        }else{
            sem_wait(SemSerf);
        }
    } 
    
    // ak je kapitán tak rozdáva lístky
    if ((position == 2) || (position == 3) || (position == 1)  )   {
        sem_wait(SemLog);
        if (position == 1){
            //ak je lod hacker rozdá lístky 3 ďaľším hackerom
            sem_post(SemHacker);
            sem_post(SemHacker);
            sem_post(SemHacker);
            // odpočet 4 hackerov z mola aj z passed
            *HackersCount = *HackersCount - 4;
            *HackersCountPassed = *HackersCountPassed - 4;
        }else if (position == 2){
            // ak je loď serf rozdá lístky 3 ďalším serfs
            sem_post(SemSerf);
            sem_post(SemSerf);
            sem_post(SemSerf);
            // odpočet 4 serfs z mola aj z passed
            *SerfsCount = *SerfsCount - 4;
            *SerfsCountPassed = *SerfsCountPassed - 4;
        }else if (position == 3){ 
            // ak je lod zmiešaná tak musí rozdať lístky podla toho aké je typ kapitán
            if (category == 0){
                // ak je kapitán hacker rozdá jeden hacker lístok a 2 serfs
                sem_post(SemHacker);
                sem_post(SemSerf);
                sem_post(SemSerf);
                // odpočet 2 hackers a 2 serfs  z mola aj passed
                *HackersCount = *HackersCount - 2;
                *SerfsCount = *SerfsCount - 2;
                *HackersCountPassed = *HackersCountPassed - 2;
                *SerfsCountPassed = *SerfsCountPassed - 2;
            }else{
                // ak je kapitán serf rozdá jeden serf lístok a 2 hackers
                sem_post(SemHacker);
                sem_post(SemHacker);
                sem_post(SemSerf);
                // odpočet 2 hackers a 2 serfs  z mola aj passed
                *HackersCount = *HackersCount - 2;
                *SerfsCount = *SerfsCount - 2;
                *HackersCountPassed = *HackersCountPassed - 2;
                *SerfsCountPassed = *SerfsCountPassed - 2;
            }
        }
        *Counter = *Counter + 1;
        // zápis o nalodení lode
        if (category == 0){
            fprintf(out, "%d  : HACK %d  : boards              : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
        }else{
            fprintf(out, "%d  : SERF %d  : boards              : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
        }
        fflush(out);
        sem_post(SemLog);  

        // uspanie kapitána na čas random medzi 0 a TravelTime = simulácia plavby lode
        if (TravelTime != 0){
            usleep(random()%TravelTime);
        }
        // po ukončení plavby povolenie 3 membrom vystúpiť
        sem_post(SemMemberExit);
        sem_post(SemMemberExit);
        sem_post(SemMemberExit);
    }

    // vystupovanie members
    if (position == 0){
        // čaká na povolenie vytúpiť od kapitána po ukončení plavby
        sem_wait(SemMemberExit);
        // ak vystúpil výpis že výstúpil
        sem_wait(SemLog);      
            *Counter = *Counter + 1;
        if (category == 0){
            fprintf(out, "%d  : HACK %d  : member exits        : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
        }else{
            fprintf(out, "%d  : SERF %d  : member exits        : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
        }
        fflush(out);
        sem_post(SemLog); 
        // signál pre kapitána že člen vystúpil
        sem_post(SemCaptainExit);
    }

    // vystupovanie kapitána
    if ((position == 2) || (position == 3) || (position == 1)  )   {
        // čakanie na vytúpenie ostatných 3 členov
        sem_wait(SemCaptainExit);
        sem_wait(SemCaptainExit);
        sem_wait(SemCaptainExit);
        // ak ostatný vystúpily z lode môže vystúpiť aj kapitán
        sem_wait(SemLog);      
            *Counter = *Counter + 1;
        if (category == 0){
            fprintf(out, "%d  : HACK %d  : captain exits       : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
        }else{
            fprintf(out, "%d  : SERF %d  : captain exits       : %d      : %d\n", *Counter, i, *HackersCount, *SerfsCount);
        }
        fflush(out);
        sem_post(SemLog);
        // povolenie na pass pre ďalšieho membra - bez neho učite nejde vytvoriť loď, takže loď sa nikdy nevytvorí skor ako jedna nedopláva       
        sem_post(SemPass);
    }

    exit(0);
}

// fuknkcia na získanie pozície na lodi
// vracia 0 ak je member
// vracia 1, 2 alebo 3 ak kapitán:
// 1 = kapitán hackers lodi
// 2 = kapitán serfs lodi
// 3 = kapitán zmiešanej lodi
void GetBoatPosition(int *position){
    // ak je počet passed hackers viac alebo rovný ako 4 vrátenie pozícia kapitána lodi hackerov 
    if (*HackersCountPassed >= 4){
        *position = 1;
    } else if ( *SerfsCountPassed >= 4){ // ak je počet passed serfs viac alebo rovný ako 4 vrátenie pozícia kapitána lodi serfov
        *position = 2;
    } else if ( (*HackersCountPassed >= 2) && (*SerfsCountPassed) >= 2 ){ // ak sú aspom 2 serfovia a zároven aspom 2 hackery, získa pozíciu kapitána zmiešanej lodi
        *position = 3;
    } else {
        // ak sa lod nedá vytvoriť tak dáva pass ďalšiemu členovi a dostáva pozíciu 0 =  member
        sem_post(SemPass);
        *position = 0;
    }


}

int createall(){
    // počítanie chýb
    int x = 0;

    //otvorenie semaforov a overenie či bolo úspešné
    if ( ((SemLog = sem_open("/semafor_log", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) ||
            ((SemPass = sem_open("/sem_pass", O_CREAT, 0666, 1)) == SEM_FAILED) ||          
            ((SemHacker = sem_open("/sem_hacker", O_CREAT, 0666, 0)) == SEM_FAILED) ||        
            ((SemSerf = sem_open("/sem_serf", O_CREAT, 0666, 0)) == SEM_FAILED) ||
            ((SemCaptainExit = sem_open("/sem_captain_exit", O_CREAT, 0666, 0)) == SEM_FAILED) ||
            ((SemMemberExit = sem_open("/sem_member_exit", O_CREAT, 0666, 0)) == SEM_FAILED))
        {
        fprintf(stderr, "sem open error\n");
        x++;
    }

    // otvorenie zdielanej pamäti a overenie úspešnosti
    if (((shmcounter = shm_open("/shmcounter", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1) ||
        ((hackcount = shm_open("/hackcount", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1) ||
        ((serfcount = shm_open("/serfcount", O_CREAT | O_EXCL | O_RDWR, 0644)) == -1) ||
        ((hackcountpassed = shm_open("/hackerspassed", O_CREAT | O_EXCL | O_RDWR, 0666)) == -1) ||
        ((serfcountpassed = shm_open("/serfspassed", O_CREAT | O_EXCL | O_RDWR, 0666)) == -1)
    ){
        fprintf(stderr, "shm open error\n");
        x++;
    }
    // nastavenie veľkosti zdielanej pamäti a overenie úspešnosti
    if (((ftruncate(shmcounter, sizeof(int))) == -1) ||
        ((ftruncate(hackcount, sizeof(int))) == -1) ||
        ((ftruncate(serfcount, sizeof(int))) == -1)  ||
        ((ftruncate(hackcountpassed, sizeof(int))) == -1) ||
        ((ftruncate(serfcountpassed, sizeof(int))) == -1) 
    ){
        fprintf(stderr, "ftruncate error\n");
        x++;
    } 
    // map zdielanej pamäti a overenie úspešnosti
    if(((Counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmcounter, 0))== MAP_FAILED) ||
        ((HackersCount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hackcount, 0))== MAP_FAILED) ||
        ((SerfsCount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, serfcount, 0))== MAP_FAILED) ||
        ((HackersCountPassed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hackcountpassed, 0))== MAP_FAILED) ||
        ((SerfsCountPassed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, serfcountpassed, 0))== MAP_FAILED)
    ){
        fprintf(stderr, "mmap error\n");
        x++;
    }
    return x;
}

void killall(){
    // uzavretie semaforov a overenie úspešnosti
    if(((sem_close(SemLog)) == -1) ||
       ((sem_close(SemHacker)) == -1) ||
       ((sem_close(SemPass)) == -1) ||
       ((sem_close(SemSerf)) == -1) ||
       ((sem_close(SemMemberExit)) == -1) ||
       ((sem_close(SemCaptainExit)) == -1) ){
        fprintf(stderr, "sem close error\n");
    }
    // unlink semaforov a overenie úspešnosti
    if ( (sem_unlink("/semafor_log") == -1) || 
        (sem_unlink("/sem_hacker") == -1) || 
        (sem_unlink("/sem_pass") == -1) || 
        (sem_unlink("/sem_serf") == -1) || 
        (sem_unlink("/sem_member_exit") == -1) || 
        (sem_unlink("/sem_captain_exit") == -1) 
    ){
        fprintf(stderr, "sem unlink error\n");
    }
    
    // unmap zdielanej pamäti a overenie úspešnosti
    if ((munmap(Counter, sizeof(int)) != 0) ||
        (munmap(HackersCount, sizeof(int)) != 0) ||
        (munmap(SerfsCount, sizeof(int)) != 0) ||
        (munmap(HackersCountPassed, sizeof(int)) != 0) ||
        (munmap(SerfsCountPassed, sizeof(int)) != 0) 
    ){
        fprintf(stderr, "munmap error\n");
    }

    // unlink zdielanej pamäti a overneie úspešnosti
    if( (shm_unlink("/shmcounter") == -1) ||
        (shm_unlink("/hackcount") == -1) ||
        (shm_unlink("/serfcount") == -1)  ||
        (shm_unlink("/hackerspassed") == -1) ||
        (shm_unlink("/serfspassed") == -1)
    ){
        fprintf(stderr, "shm unlink error\n");
    }

    // uzavretie zdielanej pamäti a overenie úspešnosti
    if( (close(shmcounter) == -1) ||
        (close(hackcount) == -1) ||
        (close(serfcount) == -1) ||
        (close(hackcountpassed) == -1) ||
        (close(serfcountpassed) == -1)  
     ){
        fprintf(stderr, "shm close error\n");
    }
    exit(0);

}