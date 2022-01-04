/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include "filesys.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}
void IncreaseCounter()
{
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}
char *User2Kernel(int Viraddr, int limit) // Doc o nho A trong user space chua chuoi nhap sang kernel space B, tra dia chi ve B
{
  int i;
  int oneChar;
  char *kernelBuf = new char[limit + 1]; // need for terminal string
  if (kernelBuf == NULL)
    return kernelBuf;
  memset(kernelBuf, 0, limit + 1);
  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(Viraddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    if (oneChar == 0)
      break;
  }
  return kernelBuf;
}
// Doc chuoi A tu kernel space va ghi vao bo nho user space chuoi B va tra ve vi tri dau tien trong bo nho cua B
int kernel2user(int virAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

int SysReadNum()
{
  int sign = 1;
  int res = 0;
  char ch;
  ch = kernel->synchConsoleIn->GetChar(); //Doc 1 ki tu nhap
  if (ch == '-')
  { //Kiem tra ki tu dau tien kiem tra am hoac duong
    sign = -1;
    ch = kernel->synchConsoleIn->GetChar();
  }
  else if (ch == '+')
  {
    sign = 1;
    ch = kernel->synchConsoleIn->GetChar();
  }
  while (ch != '\n')
  { // Neu go enter thi ket thuc nhap
    if ('0' <= ch && ch <= '9')
    { //Neu nhap co ki tu khong phai so thi tra ve 0
      res += (int)ch - 48;
      res *= 10;
    }
    else
    {
      res = 0;
      break;
    }
    ch = kernel->synchConsoleIn->GetChar();
  }
  res *= sign;
  res /= 10;
  return res;
}

void SysPrintNum(int number)
{
  if (number == 0)
  {
    kernel->synchConsoleOut->PutChar('0');
    return;
  }
  if (number < 0)
  { //Neu so am thi in ki tu "-" ra
    kernel->synchConsoleOut->PutChar('-');
    number *= -1;
  }
  int maxlen = 100;
  char *numStr = new char[maxlen];
  int i = 0, j;
  while (number != 0)
  { // Luu tung so vao mang ky tu
    numStr[i] = (char)(48 + number % 10);
    number /= 10;
    ++i;
  }
  numStr[i] = '\0'; // Gan ky tu ket thuc
  for (j = i - 1; j >= 0; --j)
  { // In theo thu tu nguoc lai
    kernel->synchConsoleOut->PutChar(numStr[j]);
  }
  delete[] numStr;
}

int GetSizeOfString(char *buffer)
{
  int i = 0;
  while (buffer[i] != '\0')
    ++i;
  return i;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
