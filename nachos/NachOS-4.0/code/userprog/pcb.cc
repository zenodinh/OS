#include "pcb.h"
#include "utility.h"
#include "kernel.h"
#include "thread.h"
#include "addrspace.h"

void StartProcess(int id)
{
    char *fileName = kernel->pTab->GetName(id);

    AddrSpace *space;
    space = new AddrSpace(fileName);

    if (space == NULL)
    {
        return;
    }
    space->Execute(); // chay chuong trinh
    ASSERTNOTREACHED();
}

PCB::PCB(int id)
{
    if (id == 0)
        this->parentID = -1;
    else
        this->parentID = kernel->currentThread->processID;

    this->exitcode = 0;
    this->thread = NULL;

    this->joinsem = new Semaphore("joinsem", 0);
    this->exitsem = new Semaphore("exitsem", 0);
    this->multex = new Semaphore("multex", 1);
}

PCB::~PCB()
{
    if (joinsem != NULL)
        delete this->joinsem;
    if (exitsem != NULL)
        delete this->exitsem;
    if (multex != NULL)
        delete this->multex;
    if (thread != NULL)
    {
        this->thread->FreeSpace();
        this->thread->Finish();
    }
}

int PCB::GetID() { return this->thread->processID; }
int PCB::GetExitCode() { return this->exitcode; }
void PCB::SetExitCode(int ec) { this->exitcode = ec; }

//block process, cho join release
void PCB::JoinWait()
{
    joinsem->P();
}

//giai phong joinwait.
void PCB::JoinRelease()
{
    joinsem->V();
}

// block, cho exit release
void PCB::ExitWait()
{
    exitsem->P();
}

// giai phong exit wait
void PCB::ExitRelease()
{
    exitsem->V();
}

void PCB::SetFileName(char *fn) { strcpy(ProcessName, fn); }
char *PCB::GetFileName() { return this->ProcessName; }

int PCB::Exec(char *filename, int pid)
{
    multex->P();
    this->thread = new Thread(filename);

    if (this->thread == NULL)
    {
        printf("Loi PCB::Exec khong tao duoc thread moi!!\n");
        multex->V();
        return -1;
    }

    this->thread->processID = pid;
    this->parentID = kernel->currentThread->processID;

    this->thread->Fork((VoidFunctionPtr)StartProcess, (void *)pid);

    multex->V();

    return pid;
}