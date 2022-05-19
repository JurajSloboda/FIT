#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "cellMatrix.h"

cellMatrix::cellMatrix(int size, int timeofSim, int minimumCureTime, int maximumCureTime, int spreadChance, double deathRate){
    //some requirearad things
    this->size = size;
    this->timeofSim = timeofSim;
    this->minimumCureTime = minimumCureTime;
    this->maximumCureTime = maximumCureTime;
    this->spreadChance = spreadChance;
    this->deathRate = deathRate;
    
    //optional incubation
    this->minimumIncubationTime = 0;
    this->maximumIncubationTime = 0;
    this->incubationSpreadChance = 0;

    //optional mask
    this->wearingMaskChance = 0;

    //quarantine
    this->quarantine = 0;

};

cellMatrix:: ~cellMatrix(){

};

void cellMatrix::setIncubation(int minimumIncubationTime, int maximumIncubationTiem, int incubationSpreadChance){
    this->minimumIncubationTime = minimumIncubationTime;
    this->maximumIncubationTime = maximumIncubationTiem;
    this->incubationSpreadChance = incubationSpreadChance;
}

void cellMatrix::setMask( int wearingMaskChance){
    this->wearingMaskChance = wearingMaskChance;
}

void cellMatrix::setQuarantine(int type){
    this->quarantine = type;
}

void cellMatrix::setSocialDistance(int socialdistance){
    this->socialdistance = socialdistance;
}

void cellMatrix::fillMatrix(){
    int x,y;
    cell Cell;
    for (x = 0; x < this->size; x++){
        for(y = 0; y < this->size; y++){
            Cell.setData(x,y,HEALTHY);
            matrix.push_back(Cell);
        }
    }
    int midPosition = (size/2)* this->size + (size/2);
    //cout << midPosition << endl;
    matrix[midPosition].state = INFECTEDINCUBATION;
    matrix[midPosition].setCureTime(this->minimumCureTime, this->maximumCureTime);
}

void cellMatrix::updateMatrix(){
    oldmatrix = matrix;
    double notSpreadChance;
    double spreadChanceforActualCell;
    int randomNumber;
    srand (time(NULL));

    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            
            //remove quarantine if longer than maxincubationtime
            if(this->quarantine > 0){
                if(matrix[x*this->size + y].quarantineTime == this->maximumIncubationTime){

                    matrix[x*this->size + y].quarantined == 0;
                    matrix[x*this->size + y].resetQuarantineTime();

                }else {
                    matrix[x*this->size + y].quarantineTime++;
                }
            }
            
            //if healthz calculate spread chance depending on neighbours and set incubation or quarantine
            if(oldmatrix[x*this->size + y].state == HEALTHY){

                notSpreadChance = getNeighbourNotSpreadChance(x - 1, y - 1) * getNeighbourNotSpreadChance (x, y-1) * getNeighbourNotSpreadChance (x+1, y-1) * getNeighbourNotSpreadChance(x-1,y)*getNeighbourNotSpreadChance(x+1,y) * getNeighbourNotSpreadChance(x-1,y+1) * getNeighbourNotSpreadChance(x, y+1)*getNeighbourNotSpreadChance(x+1,y+1);
                spreadChanceforActualCell = 1 - notSpreadChance;
                randomNumber = rand() % 100 + 1;
    
                if( randomNumber <= (int) (spreadChanceforActualCell * 100)){
                    
                    if (this->maximumIncubationTime > 0 ){

                        matrix[x*size + y].setState(INFECTEDINCUBATION);
                        matrix[x*size + y].setIncubationTime(this->minimumIncubationTime, this->maximumIncubationTime);

                    }else {

                        matrix[x*size + y].setState(INFECTEDSICK);
                        matrix[x*size + y].setCureTime(this->minimumCureTime, this->maximumCureTime);
                        matrix[x*size + y].setDeathChance(this->deathRate);

                    }
                    continue;
                }

            } else if (oldmatrix[x*this->size + y].state == INFECTEDINCUBATION){

                if (matrix[x*this->size + y].actuallTimeIncubation >= matrix[x*this->size + y].incubationTime){
                    
                    matrix[x*size + y].setState(INFECTEDSICK);
                    matrix[x*size + y].setCureTime(this->minimumCureTime, this->maximumCureTime);
                    matrix[x*size + y].setDeathChance(this->deathRate);

                } else {
                    matrix[x*size + y].actuallTimeIncubation++;
                }

            } else if (oldmatrix[x*this->size + y].state == INFECTEDSICK){
                
                
                //quarantine onlz sick person
                if(this->quarantine == QUARANTINEONLYINFECTED){

                    randomNumber = rand() % 100 + 1;
                    //20% chance of being asymptomatic 
                    if(randomNumber < 80){
                        quarantineCell(x,y);
                    }
                //quarantine everyone in contact
                } else if(this-> quarantine == QUARANTINENEIGHBOURS){

                    randomNumber = rand() % 100 + 1;
                    //20% chance of being asymptomatic 
                    if(randomNumber < 80){
                        quarantineNeighbours(x,y);
                    }

                }

                randomNumber = rand() % 10000 + 1;

                if( randomNumber <= 10000 * matrix[x*this->size + y].deathChance ){
                    matrix[x*this->size + y].setState(DEAD);

                }else if(matrix[x*this->size + y].sicktime == matrix[x*this->size + y].cureTime){
                    matrix[x*this->size + y].setState(CURED);
                } else {
                    matrix[x*this->size + y].sicktime++;
                }
            }

            
        }
    }
}

void cellMatrix::quarantineNeighbours(int x, int y){
    quarantineCell(x-1,y-1);
    quarantineCell(x-1,y);
    quarantineCell(x-1,y+1);
    quarantineCell(x,y-1);
    quarantineCell(x,y);
    quarantineCell(x,y+1);
    quarantineCell(x+1,y-1);
    quarantineCell(x+1,y);
    quarantineCell(x+1,y+1);

}

void cellMatrix::quarantineCell(int x, int y){
    if((x < 0)||(x>=this->size)||(y<0)||(y>=this->size)){
        return;
    }
    matrix[x*this->size + y].quarantined = 1;
    matrix[x*this->size + y].resetQuarantineTime();
}


double cellMatrix::getNeighbourNotSpreadChance(int x, int y){
    //srand (time(NULL));
    double result;
    int randomNumber;

    if((x < 0)||(x>=this->size)||(y<0)||(y>=this->size)){
        result = 0;
    } else if (oldmatrix[x*this->size + y].state == HEALTHY) {
        result = 0;
    } else if (oldmatrix[x*this->size + y].state == INFECTEDINCUBATION){
        result =  (double) incubationSpreadChance/100;
    } else if (oldmatrix[x*this->size + y].state == INFECTEDSICK){
        result =  (double) spreadChance/100;
    } else if (oldmatrix[x*this->size + y].state == DEAD){
        result = 0;
    } else if (oldmatrix[x*this->size + y].state == CURED){
        result = 0;
    }
    
    // 2 checks for wearing mask 
    randomNumber = rand() % 100 + 1;
    if (randomNumber <= wearingMaskChance){
        result = result * 0.65;
    }
    randomNumber = rand() % 100 + 1;
    if (randomNumber <= wearingMaskChance){
        result = result * 0.65;
    }

    //check for social distance, reduce spread bz 90%
    randomNumber = rand() % 100 +1;
    if(randomNumber <= socialdistance){
        result = result * 0.1;
    }

    if (oldmatrix[x*this->size + y].quarantined > 0){
        result = 0;
    }

    result = 1 - result; //need not spreading chance
    //cout << result << endl;
    return result;
}

void cellMatrix::PrintCells(){
    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            cout << this->matrix[x*size + y].state;
        }
        cout << endl;
    }
    
}

double cellMatrix::getHealthyPercentage(){
    int healthy = 0;
    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            if(matrix[x*size + y].state == HEALTHY){
                healthy++;
            }
        }
    }
    return (double) healthy/(size*size);
}

double cellMatrix::getInfectedPercentage(){
    int infecetd = 0;
    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            if(matrix[x*size + y].state == INFECTEDSICK){
                infecetd++;
            }
        }
    }
    return (double) infecetd/(size*size);
}

double cellMatrix::getInfectedIncubationPercentage(){
    int infectedInc = 0;
    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            if(matrix[x*size + y].state == INFECTEDINCUBATION){
                infectedInc++;
            }
        }
    }
    return (double) infectedInc/(size*size);
}

double cellMatrix::getCuredPrecentage(){
    int cured = 0;
    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            if(matrix[x*size + y].state == CURED){
                cured++;
            }
        }
    }
    return (double) cured/(size*size);
}

double cellMatrix::getDeadPercentage(){
    int dead = 0;
    for(int x = 0; x < this->size; x++){
        for (int y = 0; y < this->size; y++){
            if(matrix[x*size + y].state == DEAD){
                dead++;
            }
        }
    }
    return (double) dead/(size*size);
}