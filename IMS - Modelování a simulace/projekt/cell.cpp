#include "cell.h"

#include <cstdlib>
#include <math.h>
#include <iostream>

cell::cell(){
    x = -1;
    y = -1;
    state = -1;
    sicktime = 0;
    cureTime = 0;
    actuallTimeIncubation = 0;
    quarantined = 0;
    quarantineTime = 0;
    incubationTime = 0;
};

cell::~cell(){
};


void cell::setData(int x, int y, int state){
    this->x = x;
    this->y = y;
    this->state = state;
}

void cell::setState(int state){
    this->state = state;
}

void cell::setCureTime(int minimumTime, int maximumTime){
    this->cureTime = minimumTime + rand() % (maximumTime - minimumTime + 1);
}

void cell::setIncubationTime(int minimumTime, int maximumTime){
    this->incubationTime = minimumTime + rand() % (maximumTime - minimumTime + 1);
}

void cell::resetQuarantineTime(){
    this->quarantineTime = 0;
}

void cell::setDeathChance(double deathRate){
    
    double deathRatetmp = 1 - deathRate;
    deathRatetmp = pow(deathRatetmp, 1/(double)this->cureTime);
    this->deathChance = 1 - deathRatetmp;
}