/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int main()
{
    int A[10];
    int size;
    int i, j, temp, code;
    PrintString("Nhap size: (<=100)");
    size = ReadNum();
    //Nhap mang
    for (i = 0; i < size; i++)
    {
        PrintString("Nhap A[");
        PrintNum(i + 1);
        PrintString("]=");
        A[i] = ReadNum();
    }
    //Sap xep
    PrintString("Go 1: Sap xep Tang dan !!!\nGo 2: Sap xep Giam dan !!!\n");
    code = ReadNum();
    if (code == 1)
    {
        for (i = 0; i < size - 1; i++)
        {
            for (j = i + 1; j < size; j++)
            {
                if (A[i] > A[j])
                {
                    temp = A[j];
                    A[j] = A[i];
                    A[i] = temp;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < size - 1; i++)
        {
            for (j = i + 1; j < size; j++)
            {
                if (A[i] < A[j])
                {
                    temp = A[j];
                    A[j] = A[i];
                    A[i] = temp;
                }
            }
        }
    }

    //Xuat mang
    for (i = 0; i < size; i++)
    {
        PrintString("A[");
        PrintNum(i + 1);
        PrintString("]=");
        PrintNum(A[i]);
        PrintChar('\n');
    }
    PrintChar('\n');
    Halt();
    /* not reached */
}