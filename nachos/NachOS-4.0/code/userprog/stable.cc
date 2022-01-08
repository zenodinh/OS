#include "stable.h"

//////////////////////////////// Cai dat cho lop Sem ////////////////////////////////
Sem::Sem(char *na, int i)
{
    strcpy(this->name, na);
    sem = new Semaphore(this->name, i);
}

Sem::~Sem()
{
    if (sem)
        delete sem;
}

void Sem::wait()
{
    sem->P();
}

void Sem::signal()
{
    sem->V();
}

char *Sem::GetName()
{
    return this->name;
}

//////////////////////////////// Cai dat cho lop Stable ////////////////////////////////
STable::STable()
{
    this->bm = new Bitmap(MAX_SEMAPHORE);

    for (int i = 0; i < MAX_SEMAPHORE; ++i)
        this->semTab[i] = NULL;
}

STable::~STable()
{
    if (this->bm)
    {
        delete this->bm;
        this->bm = NULL;
    }
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
    {
        if (this->semTab[i])
        {
            delete this->semTab[i];
            this->semTab[i] = NULL;
        }
    }
}

int STable::Create(char *name, int init)
{
    // Kiem tra semahore da ton tai hay chua?
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
    {
        if (bm->Test(i)) // Co ton tai
        {
            if (strcmp(name, semTab[i]->GetName()) == 0)
                return -1; // Truong hop da ton tai
        }
    }
    // Tim slot trong cua semTab
    int freeSlot = this->FindFreeSlot();
    if (freeSlot < 0)
        return -1; // Truong hop khong tim thay vi tri strong
    // Truong hop tim thay
    this->semTab[freeSlot] = new Sem(name, init);
    return 0;
}

int STable::Wait(char *name)
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
    {
        // Kiem tra vi tri i da duoc nap semaphore chua?
        if (bm->Test(i))
        {
            // Truong hop da co thi kiem tra tham so name va name cua Sem trong semTab
            if (strcmp(name, semTab[i]->GetName()) == 0)
            {
                // Neu ton tai thi cho semaphore down()
                semTab[i]->wait();
                return 0; // Tra ve 0 khi da wait name thanh cong
            }
        }
    }
    // Truong hop khong tim thay name hoac khong the wait duoc
    printf("Loi STable::Wait: Khong ton tai semaphore = %s!!\n", name);
    return -1;
}

int STable::Signal(char *name)
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
    {
        // Kiem tra vi tri i da duoc nap semaphore chua?
        if (bm->Test(i))
        {
            // Truong hop da co thi kiem tra tham so name va name cua Sem trong semTab
            if (strcmp(name, semTab[i]->GetName()) == 0)
            {
                // Neu ton tai thi cho semaphore up()
                semTab[i]->signal();
                return 0; // Tra ve 0 khi da signal name thanh cong
            }
        }
    }
    // Truong hop khong tim thay name hoac khong the signal duoc
    printf("Loi STable::Signal: Khong ton tai semaphore = %s!!\n", name);
    return -1;
}

int STable::FindFreeSlot()
{
    return this->bm->FindAndSet();
}