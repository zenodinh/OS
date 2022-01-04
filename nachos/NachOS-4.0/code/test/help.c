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

int
main()
{
    char* instruction={"***     Nhom He Dieu Hanh     ***\n19120336 Dinh Trong Quan\n19120384 Nguyen Trung Thoi\n19120296 Do Hoai Nam\n***********************\n---ascii: Chuong trinh se in ra cac ky tu co ma ascii tu 32 den 126 (gom cac ky tu in ra duoc)\n---sort: Chuong trinh Bubble Sort sap xep mang bang cach chay 2 vong lap va doi vi tri cac phan tu voi nhau de thoa man tang dan hoac giam dan\n"};
    PrintString(instruction);
    Halt();
    /* not reached */
}