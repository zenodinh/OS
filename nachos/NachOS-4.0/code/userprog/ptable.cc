#include "ptable.h"
#include "kernel.h"
#include "openfile.h"

PTable::PTable()
{
    bm = new Bitmap(MAXPROCESS);
    bmsem = new Semaphore("bmsem", 1);

    for (int i = 0; i < MAXPROCESS; i++)
    {
        pcb[i] = NULL;
    }

    bm->Mark(0);

    pcb[0] = new PCB(0);
    psize = MAXPROCESS;
}

PTable::PTable(int size)
{
    psize = size;
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem", 1);

    for (int i = 0; i < MAXPROCESS; ++i)
        pcb[i] = NULL;
    bm->Mark(0);
    pcb[0] = new PCB(0);
}

PTable::~PTable()
{
    if (bm != NULL)
        delete bm;
    if (bmsem != NULL)
        delete bmsem;
    for (int i = 0; i < MAXPROCESS; i++)
    {
        if (bm->Test(i))
        {
            delete pcb[i];
            pcb[i] = NULL;
        }
    }
}

int PTable::ExecUpdate(char *name)
{
    bmsem->P(); //chi nap 1 tien trinh vao mot thoi diem
                // Kiem tra name co hop le khong
    if (name == NULL)
    {
        bmsem->V();
        return -1;
    }
    //Kiem tra chuong trinh co goi thuc thi chinh no khong
    if (strcmp(name, kernel->currentThread->getName()) == 0)
    {
        printf("Loi PTable::ExecUpdate: khong the goi chinh no!!\n");
        bmsem->V();
        return -1;
    }
    //Kiem tra con slot trong khong
    int freeSlot = this->GetFreeSlot();
    if (freeSlot < 0)
    {
        printf("Loi PTable::ExecUpdate: Da vuot qua 10 tien trinh!!\n");
        bmsem->V();
        return -1;
    }
    pcb[freeSlot] = new PCB(freeSlot);
    pcb[freeSlot]->SetFileName(name);
    pcb[freeSlot]->parentID = kernel->currentThread->processID;

    int pid = pcb[freeSlot]->Exec(name, freeSlot);

    bmsem->V();
    return pid;
}

int PTable::ExitUpdate(int ec)
{
    //Kiem tra pID co ton tai khong
    int pID = kernel->currentThread->processID;
    int parID = pcb[pID]->parentID;
    if (!IsExist(pID))
    {
        printf("\nLoi: Tien trinh khong ton tai !!!\n");
        return -1;
    }
    //////////////////////////////////////////////////////////////

    //Neu la main process thi Halt()
    if (pID == 0)
    {
        kernel->interrupt->Halt();
        return 0;
    }
    /////////////////////////////////////////////////////////////

    pcb[pID]->SetExitCode(ec);

    pcb[pID]->JoinRelease();
    pcb[pID]->ExitWait();
    Remove(pID);

    return ec;
}

int PTable::JoinUpdate(int pID)
{
    if (pID < 0 || pID > 9)
    {
        printf("\nLoi: Khong ton tai process: id = %d\n", pID);
        return -1;
    }

    if (pcb[pID] == NULL)
    {
        printf("Loi: Khong ton tai process id nay!");
        return -1;
    }

    //kiem tra tien trinh dang chay co la cha cua tien trinh can join hay khong
    if (kernel->currentThread->processID != pcb[pID]->parentID || pcb[pID]->GetID() == kernel->currentThread->processID)
    {
        printf("\nLoi: Ko duoc phep join vao tien trinh khong phai cha cua no !!!\n");
        return -1;
    }

    pcb[pID]->JoinWait(); //doi den khi tien trinh con ket thuc

    int ec = pcb[pID]->GetExitCode();

    if (ec != 0)
    {
        printf("\nProcess exit with exitcode EC = %d ", ec);
        return -1;
    }

    pcb[pID]->ExitRelease(); //cho phep tien trinh con ket thuc

    return 0;
}

void PTable::Remove(int pID)
{
    if (pID < 0 || pID > 9)
        return;
    if (bm->Test(pID))
    {
        bm->Clear(pID);
        delete pcb[pID];
    }
}

//----------------------------------------------------------------------------------------------
int PTable::GetFreeSlot()
{
    return bm->FindAndSet();
}

bool PTable::IsExist(int pID)
{
    if (pID < 0 || pID > 9)
        return 0;
    return bm->Test(pID);
}

char *PTable::GetName(int pID)
{
    if (pID >= 0 && pID < 10 && bm->Test(pID))
        return pcb[pID]->GetFileName();
}
