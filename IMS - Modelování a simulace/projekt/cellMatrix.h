#ifndef CELL_MATRIX_H
#define CELL_MATRIX_H

#include "cell.h"
#include <vector>

#define HEALTHY 1
#define INFECTEDINCUBATION 2
#define INFECTEDSICK 3
#define DEAD 4
#define CURED 5

#define QUARANTINEONLYINFECTED 101
#define QUARANTINENEIGHBOURS 102

using namespace std;

class cellMatrix {
public:
    //constructor, destrucwtrodsda
    cellMatrix(int size, int time, int minimumCureTime, int maximumCureTime, int spreadChance, double deathRate);
    ~cellMatrix();

    //base settings for matrix
    void fillMatrix();
    void setIncubation(int minimumIncubationTime, int maximumIncubationTime, int incubationSpreadChance);
    void setRandomSpreadChance(int);
    void setQuarantine(int type);

    //
    void updateMatrix();
    
    //print all the cells (Merlin)
    void PrintCells();

    //setmask
    void setMask(int);

    //get some stats
    double getHealthyPercentage();
    double getInfectedPercentage();
    double getInfectedIncubationPercentage();
    double getCuredPrecentage();
    double getDeadPercentage();

    void setSocialDistance(int);

    //some other 
    int size;
    int spreadChance;
    double deathRate;
    int minimumCureTime;
    int maximumCureTime;
    int timeofSim;

    //optioansdal
    int minimumIncubationTime;
    int maximumIncubationTime;
    int incubationSpreadChance;

    int wearingMaskChance;

    int quarantine;

    int socialdistance;

    //Matrix
    std::vector<cell> matrix;
    std::vector<cell> oldmatrix;

private:

    void quarantineNeighbours(int x, int y);
    void quarantineCell(int x, int y);
    double getNeighbourNotSpreadChance(int x, int y);
};


#endif