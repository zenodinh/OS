// synchconsole.cc 
//	Routines providing synchronized access to the keyboard 
//	and console display hardware devices.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// SynchConsoleInput::SynchConsoleInput
//      Initialize synchronized access to the keyboard
//
//      "inputFile" -- if NULL, use stdin as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleInput::SynchConsoleInput(char *inputFile)
{
    consoleInput = new ConsoleInput(inputFile, this);
    lock = new Lock("console in");
    waitFor = new Semaphore("console in", 0);
}

//----------------------------------------------------------------------
// SynchConsoleInput::~SynchConsoleInput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleInput::~SynchConsoleInput()
{ 
    delete consoleInput; 
    delete lock; 
    delete waitFor;
}

//----------------------------------------------------------------------
// SynchConsoleInput::GetChar
//      Read a character typed at the keyboard, waiting if necessary.
//----------------------------------------------------------------------

char
SynchConsoleInput::GetChar()
{
    char ch;

    lock->Acquire();
    waitFor->P();	// wait for EOF or a char to be available.
    ch = consoleInput->GetChar();
    lock->Release();
    return ch;
}

//----------------------------------------------------------------------
// Ham tui em cai dat
// SynchConsoleInput:: GetNum
// Read an int at keyboard
//----------------------------------------------------------------------

int
SynchConsoleInput::ReadNum()
{
	//Nhan chuoi
	char* buffer;
     int MAX_BUFFER = 255;
     buffer = new char[MAX_BUFFER + 1];
	int eolncond = false;
	char ch;
	int index = 0;
	lock->Acquire();
	while (index < MAX_BUFFER && eolncond == false)
	{
		do{
			waitFor->P();
			ch = consoleInput->GetChar();
		} while (ch == EOF);
		if (ch == '\012' || ch == '\001')
		{
			eolncond = TRUE;
		}
		else{
			buffer[index] = ch;
			index++;
		}
	}
	lock->Release();
	
	
     int number = 0; 			                     
     bool isNegative = false; // Bien kiem tra tinh am duong
     int firstNumIndex = 0;
     int lastNumIndex = 0;
	 int pivot=0; 
	 
	 // Kiem tra tinh hop le cua so nguyen buffer
	  for(pivot; pivot<index; pivot++)
	  {
		 if(buffer[pivot] == ' ' &&  (buffer[pivot-1]==' ' || pivot==0) ) //tinh huong nhap "      12"
		 {
			 firstNumIndex++;
			 lastNumIndex++;
		 }
		 else if(buffer[pivot] == '-' && isNegative !=true) //tinh huong nhap so am. Chi xet truong hop 1 dau tru. Vd: "-3", khong xet "--3"
		{
			isNegative = true;
			firstNumIndex = pivot+1;
			lastNumIndex = pivot+1; 
		}
		else if (buffer[pivot] == '.' && buffer[pivot-1] >= '0' && buffer[pivot-1] <= '9'&&pivot>0) // Tinh huong nhap "12.000"
		{
                int j = pivot + 1;
                for(; j < index; j++)
                 {
				     // Neu xuat hien so khac 0 sau dau phay. Vd: "12.010"
                     if(buffer[j] != '0')
                     {
                         delete buffer;
                         return 0;
                     }
				}
                lastNumIndex = pivot - 1;				
                 break;
		}
		else if(buffer[pivot] < '0' || buffer[pivot] > '9')//Neu xuat hien ky tu khong phai so. Vd: "12d8"
        {
             delete buffer;
              return 0;
        }
        lastNumIndex = pivot; 
	  }
	  
	  //Kiem tra so nguyen co qua gioi han trong C
	int length=lastNumIndex-firstNumIndex+1;
	char maxOfRange[] ={'2','1','4','7','4','8','3','6','4','7'};
	bool inRange=false;
	
	if(length<10)
	{
		inRange=true;
	}
	else if(length==10)
	{
		int equalCheck=0;
		for(int i=firstNumIndex;i<=lastNumIndex;i++)
		{
			if( buffer[i]<maxOfRange[i-firstNumIndex])
			{
				inRange=true;
				break;
			}
			else if( buffer[i]>maxOfRange[i-firstNumIndex])
			{
				break;
			}
			else if(buffer[i]==maxOfRange[i-firstNumIndex])
			{
				equalCheck++;
			}
			if(equalCheck==10)
			{
				inRange=true;
				break;
			}
		}
	}
	  
	 if(inRange==false) //Neu qua gioi han thi tra ve 0
	 {
		 delete buffer;
		 return 0;
	 }		 
	  
	  
        // Neu la so nguyen hop le, doi tu chuoi thanh so
        for(int i = firstNumIndex; i<=lastNumIndex; i++)
        {
            number = number * 10 + (int)(buffer[i] - 48); 

        }
		if(isNegative) //Neu la so am thi nhan them -1
		{
			number=number*-1;
		}
		
                    
		delete buffer;
		return number;
}

//----------------------------------------------------------------------
// Ham tui em cai dat
// SynchConsoleInput:: Get string
// Read a string at keyboard
//----------------------------------------------------------------------

void
SynchConsoleInput::ReadString(char buffer[], int length)
{
	int index;
	char ch;
	index = 0;
	lock->Acquire();
	while (index < length)
	{
		waitFor->P();
		ch = consoleInput->GetChar();
		if (ch == '\001' || ch == '\012')
		{
			break;
		}
		else if (ch == EOF)
		{
			DEBUG(dbgFile, "EOF.\n");
			break;
		}
		else{
			buffer[index] = ch;
			index++;
		}
	}
	lock->Release();
}

//----------------------------------------------------------------------
// Ham tui em cai dat
// SynchConsoleInput:: Read
// Read a string at keyboard
//----------------------------------------------------------------------

int
SynchConsoleInput::Read(char buffer[], int length)
{
	int index;
	char ch;
	index = 0;
	lock->Acquire();
	while (index < length)
	{
		waitFor->P();
		ch = consoleInput->GetChar();
		if (ch == EOF)
		{
			DEBUG(dbgFile, "EOF.\n");
			break;
		}
		else if (ch == '\001' || ch == '\012')
		{
			break;
		}
		else{
			buffer[index] = ch;
			index++;
		}
	}
	lock->Release();
	if (ch == '\001')
		return -1;
	else if (ch == EOF)
		return -2;
	else
		return index;
}

//----------------------------------------------------------------------
// SynchConsoleInput::CallBack
//      Interrupt handler called when keystroke is hit; wake up
//	anyone waiting.
//----------------------------------------------------------------------

void
SynchConsoleInput::CallBack()
{
    waitFor->V();
}

//----------------------------------------------------------------------
// SynchConsoleOutput::SynchConsoleOutput
//      Initialize synchronized access to the console display
//
//      "outputFile" -- if NULL, use stdout as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleOutput::SynchConsoleOutput(char *outputFile)
{
    consoleOutput = new ConsoleOutput(outputFile, this);
    lock = new Lock("console out");
    waitFor = new Semaphore("console out", 0);
}

//----------------------------------------------------------------------
// SynchConsoleOutput::~SynchConsoleOutput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleOutput::~SynchConsoleOutput()
{ 
    delete consoleOutput; 
    delete lock; 
    delete waitFor;
}

//----------------------------------------------------------------------
// SynchConsoleOutput::PutChar
//      Write a character to the console display, waiting if necessary.
//----------------------------------------------------------------------

void
SynchConsoleOutput::PutChar(char ch)
{
    lock->Acquire();
    consoleOutput->PutChar(ch);
    waitFor->P();
    lock->Release();
}

//----------------------------------------------------------------------
// Ham tui em cai dat
// SynchConsoleInput:: PrintNum
// Read an int at keyboard
//----------------------------------------------------------------------

void
SynchConsoleOutput::PrintNum(int num)
{
   char buffer[11]; // chieu dai dai nhat cua chuoi int trong C duoi dang string
   int firstIndex = 0;
   //In ra man hinh so 0
   if(num==0)
   {
	consoleOutput->PutChar('0');	
   }
   else if (num!=0)
   {
   //Chuyen so nguyen ve dang chuoi
    if (num < 0)
   {
	   buffer[0] = '-';
	   firstIndex = 1; //chu so dau la dau '-'
	   num = num * -1; // lay so duong cho de dua vao buffer
   }
   int length = 0;
   int tempNum = num;
   //Lay chieu dai so, tempNum la copy cua num;
   while (tempNum)
   {
	   tempNum /= 10;
	   length++;
   }
   // Dua vao buffer
   buffer[length + firstIndex] = '\0';
   int i ;
   for (i = length + firstIndex - 1; i>= firstIndex;  i--) //di tu don vi tang len
   {
	   buffer[i]  = char(num % 10  + '0');
	   num /= 10;
   }
   // sau day nhu ham print string
	this->PrintString(buffer);
   }
  
}

//----------------------------------------------------------------------
// Ham tui em cai dat
// SynchConsoleInput:: PrintString
// Read a string at keyboard
//----------------------------------------------------------------------

void
SynchConsoleOutput::PrintString(char buffer[])
{
	if (buffer == NULL) return;
	int index = 0;
	lock->Acquire();
	do{
		consoleOutput->PutChar(buffer[index]);
		waitFor->P();
		index++;
	} while (buffer[index] != '\0');
    lock->Release(); 
}

//----------------------------------------------------------------------
// Ham tui em cai dat
// SynchConsoleInput:: Write
// Read a string at keyboard
//----------------------------------------------------------------------

int
SynchConsoleOutput::Write(char buffer[], int length)
{
	if (buffer == NULL) return 0;
	int index = 0;
	lock->Acquire();
	do{
		consoleOutput->PutChar(buffer[index]);
		waitFor->P();
		index++;
	} while (buffer[index] != '\0');
    lock->Release(); 
	return index;
}

//----------------------------------------------------------------------
// SynchConsoleOutput::CallBack
//      Interrupt handler called when it's safe to send the next 
//	character can be sent to the display.
//----------------------------------------------------------------------

void
SynchConsoleOutput::CallBack()
{
    waitFor->V();
}
