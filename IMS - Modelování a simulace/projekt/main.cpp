#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>
#include <math.h>

#include <fstream>
#include <iomanip>

#include "cellMatrix.h"

using namespace std;

void printHelp(){
    cout <<"-t X = Simulation time in days(integer)"<< endl;
    cout <<"-s X = siye of amtrix (integer)"<< endl;
    cout <<"-d X = death rate of virus (0-100)"<< endl;
    cout <<"-r X = transmissibility  rate of virus - chance of spread when in contact with anyone else (0-100)"<< endl;
    cout <<"-c X = minimum time of being cured in days (integer)"<< endl;
    cout <<"-C X = maximum time of being cured in dazs (days)" << endl;

    cout <<"Optional "<< endl;
    cout <<"-h Get some help."<< endl;
    cout <<"-v X Simulation speed (integer) - time between states (in miliseconds)"<< endl;
    cout <<"-m X Chance of wearing mask when in contact (reduce spread chance by 65% when both sides wear mask spread is reduced by ~88%) (0-100%)"<< endl;
    cout <<"-q 1/2 = Quarantine (type 1 for onlz infected cell, type 2 for all neighbours and cell)" << endl;
    cout <<"-l X = chance of social distance (integer)" << endl;

    cout << "Optional incubation time (need to set all or none)" << endl;
    cout <<"-i X = minimum time of virus incubation in dazs (integre)" << endl;
    cout <<"-I X = maximum time of virus incubastao in dazs (integer)" << endl;
    cout <<"-p X = chance of spreading virus till incubation in percent (0-100)" << endl;
}


double round(double var) { 
    double value = (int)(var * 100 + .5); 
    return (double)value / 100; 
} 

int main(int argc, char *argv[]){

    //getops varivalas
    char *cvalue = NULL;
    int c;
    opterr = 0;

    //requirede vairales
    int size = 0;
    int spreadChance = 0;
    double deathRate = 0.0;
    int deathRatetmp = 0;
    int minimumCureTime = 0;
    int maximumCureTime = 0;
    int time = 0;

    int minimumIncubationTime = 0;
    int maximumIncubationTime = 0;
    int incubationSpreadChance = 0;

    int speed = 1;

    int wearingMaskChance = 0;

    int quarantinetype = 0;

    int socialdistance = 0;

    int showstates = 0;

    std::vector<double> HealthyPercentage;
    std::vector<double> InfectedPercentage;
    std::vector<double> InfectedIncubationPercentage;
    std::vector<double> CuredPercentage;
    std::vector<double> DeadPercentage;

    while ((c = getopt (argc, argv, "t:s:d:r:c:C:hv:m:i:I:p:q:l:a")) != -1){
        switch (c){
            case 't':
                time = atoi(optarg);
                break;
            case 's':
                size = atoi(optarg);
                break;
            case 'd':
                deathRatetmp = atoi(optarg);
                break;
            case 'r':
                spreadChance = atoi(optarg);
                break;
            case 'c':
                minimumCureTime = atoi(optarg);
                break;
            case 'C':
                maximumCureTime = atoi(optarg);
                break;
            case 'h':
                printHelp();
                return 0;
                break;
            case 'v':
                speed = atoi(optarg) * 1000;
                break;
            case 'm':
                wearingMaskChance = atoi(optarg);
                break;
            case 'i':
                minimumIncubationTime = atoi(optarg);
                break;
            case 'I':
                maximumIncubationTime = atoi(optarg);
                break;
            case 'p':
                incubationSpreadChance = atoi(optarg);
                break;
            case 'q':
                quarantinetype = atoi(optarg) + 100;
                break;
            case 'l':
                socialdistance = atoi(optarg);
                break;
            case 'a':
                showstates = 1;
                break;
            case '?':
                cout <<"-h Stop it. Get some help"<< endl;
                return 1;
            default:
                cout<< "something went wrong " << endl;
                cout <<"-h Stop it. get some help"<< endl;
                return 1;
        }
    }
    
    deathRate = (double)deathRatetmp / 100;
    cellMatrix cells(size, time, minimumCureTime, maximumCureTime, spreadChance, deathRate);
    cells.setMask(wearingMaskChance);
    cells.fillMatrix();
    cells.setIncubation(minimumIncubationTime, maximumIncubationTime, incubationSpreadChance);
    cells.setQuarantine(quarantinetype);
    cells.setSocialDistance(socialdistance);

    for( int i = 0; i < time; i++){
        
        cells.updateMatrix();

        HealthyPercentage.push_back(cells.getHealthyPercentage());
        InfectedPercentage.push_back(cells.getInfectedPercentage());
        InfectedIncubationPercentage.push_back(cells.getInfectedIncubationPercentage());
        CuredPercentage.push_back(cells.getCuredPrecentage());
        DeadPercentage.push_back(cells.getDeadPercentage());

        if(showstates){
            cout << "==================================State: " << i+1 <<"============================================="  << endl;
            cells.PrintCells();
            cout << "======================================================================================="  << endl;
        }
        

        usleep(speed);
    }

    cout << "Time[days],Healthy,Incubation,Infected,Cured,Dead" << endl;
    for (int i = 0; i < HealthyPercentage.size(); i++){
        cout << i+1 << "," << round(HealthyPercentage[i]*100) << "," << round(InfectedIncubationPercentage[i]*100) << "," << round(InfectedPercentage[i]*100) << "," << round(CuredPercentage[i]*100) << "," << round(DeadPercentage[i]*100) << endl;
    }

    return 0;
}
