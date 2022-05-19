#ifndef CELL_H
#define CELL_H

using namespace std;

class cell {
public:
    cell();
    ~cell();

    void setData(int, int, int);
    void setState(int);
    void setCureTime(int, int);
    void setIncubationTime(int, int);
    void resetQuarantineTime ();
    void setDeathChance(double);

    int x;
    int y;
    int state;
    int sicktime;
    int cureTime;

    int actuallTimeIncubation;
    int incubationTime;

    int quarantined;
    int quarantineTime;

    double deathChance;
};


#endif 