#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"
#include "synch.h"

#define MAX_SEMAPHORE 10

class Semaphore; // Khoi tao truoc de compiler hieu rang lop semaphore se duoc dinh nghia sau
class Sem
{
private:
    char name[50];
    Semaphore *sem;

public:
    Sem(char *na, int i);
    ~Sem();
    void wait();
    void signal();
    char *GetName();
};

class STable
{
private:
    Bitmap *bm;
    Sem *semTab[MAX_SEMAPHORE];

public:
    STable();
    ~STable();
    int Create(char *name, int init);
    int Wait(char *name);
    int Signal(char *name);
    int FindFreeSlot();
};

#endif