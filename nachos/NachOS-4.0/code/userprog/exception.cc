// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#define FILE_NAME_LIMIT 64
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		}
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadNum:
		{
			int value = 0;
			int maxInt = 2147483647, minInt = -2147483648; // Giu gia tri lon nhat cua kieu integer
			value = SysReadNum();						   // Goi ham doc so
			if (value > maxInt)							   //Kiem tra co vuot qua gia tri Max, Min
				value = maxInt;
			if (value < minInt)
				value = minInt;
			kernel->machine->WriteRegister(2, value); //Luu vao thanh ghi bo nho
			IncreaseCounter();						  //Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintNum:
		{
			//Doc so
			int result = kernel->machine->ReadRegister(4); //Doc so da nhap
			SysPrintNum(result);						   // Goi ham in ra man hinh
			IncreaseCounter();							   //Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadChar:
		{
			char c = kernel->synchConsoleIn->GetChar(); // Doc 1 ki tu
			kernel->machine->WriteRegister(2, c);		// Luu vao thanh ghi bo nho
			IncreaseCounter();							// Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintChar:
		{
			char c = (char)kernel->machine->ReadRegister(4); //Luu ki tu da nhap vao bien
			kernel->synchConsoleOut->PutChar(c);			 //In ra man hinh
			IncreaseCounter();								 //Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_RandomNum:
		{
			srand(time(0));									 // Truyen seed
			int randomNumber = rand();						 // Goi ham random trong sysdep.cc
			kernel->machine->WriteRegister(2, randomNumber); //Ghi vao thanh ghi r2
			IncreaseCounter();								 // Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadString:
		{
			int len = kernel->machine->ReadRegister(5); //Luu length cua chuoi vao bien
			char *tmp = new char[len + 1];
			int i = 0;
			for (i = 0; i < len; i++)
			{
				tmp[i] = kernel->synchConsoleIn->GetChar(); // Doc tung ki tu vao mang ki tu
				if (tmp[i] == '\n')
					break; // Nhap enter thi ket thuc
			}
			tmp[i] = '\0'; // Gan ki tu key thuc vao cuoi chuoi
			kernel->machine->WriteRegister(2, i);
			kernel2user((int)kernel->machine->ReadRegister(4), i, tmp); //Ghi mang vao userspace
			delete[] tmp;
			IncreaseCounter(); // Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintString:
		{
			int maxLen = 500;
			char *kernelBuffer = User2Kernel((int)kernel->machine->ReadRegister(4), maxLen); //Nhan du lieu tu user space
			int i = 0;
			while (kernelBuffer[i] != 0)
			{
				kernel->synchConsoleOut->PutChar(kernelBuffer[i]); //In tung ki tu ra man hinh
				i++;
			}
			kernel->machine->WriteRegister(2, i);
			IncreaseCounter(); //Tang bien dem
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_Create:
		{
			// Cu phap: int Create(char* buffer);
			// Input: Dia chi tu vung nho user cua ten file
			// Output: Loi: -1, Thanh cong: 0
			// Tao ra file moi voi ten file la tham so dau vao
			int MaxFileLength;
			MaxFileLength = 32;
			char *filename;
			int virtAddr;
			DEBUG('a', '\n SC_CreatFile call...');
			DEBUG('a', '\n Reading filename virtual address');
			virtAddr = kernel->machine->ReadRegister(4); // Doc dia chi cua file tu thanh ghi R4
			DEBUG('a', '\n Reading filename');
			// Chuyen khong gian bo nho User sang system
			filename = User2Kernel(virtAddr, MaxFileLength + 1);

			if (strlen(filename) == 0)
			{
				printf("\n Invalid file name!");
				DEBUG('a', '\n Invalid filename!');
				kernel->machine->WriteRegister(2, -1); // Tra ve ket qua -1: that bai
				IncreaseCounter();
				return;
			}
			if (filename == NULL)
			{
				printf("\n Not enough memory in system space!");
				DEBUG('a', '\n Not enough memory in system space!');
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			DEBUG('a', '\n Finish reading filename');
			bool test;
			test = kernel->fileSystem->Create(filename);
			if (!test)
			{ // Tao file
				// that bai
				printf("\n Error occured while creating file %s", filename);
				kernel->machine->WriteRegister(2, -1);
			}
			// thanh cong
			else
			{
				kernel->machine->WriteRegister(2, 0);
			}

			delete[] filename;
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();

			break;
		}

		case SC_Open:
		{
			// Cu phap: int Open(char* filename, int type);
			// Input: ten file can mo (co the bao gom duong dan) va kieu file (0 la binh thuong con 1 la chi doc)
			// Output: OpenFileId id trong fileTable dai dien cho tap tin vua mo
			char *filename;
			int freeSlot;

			int virtAddr = kernel->machine->ReadRegister(4);
			int type = kernel->machine->ReadRegister(5);
			int filenameMaxLength = 32;
			// Chuyen du lieu tu user space sang kernel space
			filename = User2Kernel(virtAddr, filenameMaxLength + 1);
			// Kiem tra kieu file muon thuc thi co thuoc loai chi doc hoac doc va ghi khong
			if (type == OnlyRead || type == ReadAndWrite)
			{
				// Lay vi tri trong fileTable tu ham Open
				// Do ham open
				int freeSlot = kernel->fileSystem->Open(filename, type);
				// if (kernel->fileSystem->fileTable[freeSlot] != NULL)
				kernel->machine->WriteRegister(2, freeSlot);
				delete[] filename;
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			kernel->machine->WriteRegister(2, -1);

			delete[] filename;
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Close:
		{
			// Cu phap: int Close(OpenFileId id);
			// Input: id dai dien cho file dinh close
			// Output: Tra ve -1 neu that bai, 0 neu thanh cong
			OpenFileId id = kernel->machine->ReadRegister(4);
			// Kiem tra id phai thuoc bang mo ta fileTable va tap tin co ton tai khong
			if (id >= 0 && id < 10 && kernel->fileSystem->fileTable[id] != NULL)
			{
				// Ta goi ham close trong lop fileSystem de giup dong file
				int result = kernel->fileSystem->Close(id);
				kernel->machine->WriteRegister(2, result);
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			kernel->machine->WriteRegister(2, -1);
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Read:
		{
			// Cu phap: int Read(char* buffer, int size, OpenFileId id);
			// Input: Buffer dung de luu chuoi doc tu file vao, size la kich thuoc ma ta muon doc, id dai dien cho file muon doc
			// Output: tra ve -1 neu that bai, So byte doc duoc neu thanh cong
			int realSize = 0;
			int virtAddr = kernel->machine->ReadRegister(4);  // Doc vi tri cua buffer trong thanh ghi r4
			int size = kernel->machine->ReadRegister(5);	  // Doc kich thuoc chuoi yeu cau trong thanh ghi r5
			OpenFileId id = kernel->machine->ReadRegister(6); // Doc OpenFileID id tu thanh ghi r6
			char *buffer;
			int oldPos = 0;
			int newPos = 0;
			// Kiem tra id cua file phai nam trong bang mo ta file
			if (id < 0 || id > 9)
			{
				printf("\nLoi Read: Khong the truy cap file vi id = %d nam ngoai bang mo ta.\n", id);
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			// Truong hop doc du lieu tu Ouput console thi loi
			if (id == OutputConsoleOut)
			{
				printf("\nLoi Read: Khong the doc du lieu tu STDOUT (console ouput)\n");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			//Truong hop doc du lieu STDIN tu input console
			if (id == InputConsoleIn)
			{
				buffer = User2Kernel(virtAddr, size);
				buffer = new char[size + 1];
				int i;
				for (i = 0; i < size; i++)
				{
					buffer[i] = kernel->synchConsoleIn->GetChar(); // Doc tung ki tu vao mang ki tu
					if (buffer[i] == '\n')
						break; // Nhap enter thi ket thuc
				}
				buffer[i] = '\0'; // Gan ki tu key thuc vao cuoi chuoi
				kernel->machine->WriteRegister(2, i);
				kernel2user((int)kernel->machine->ReadRegister(4), i, buffer); //Ghi mang vao userspace
				delete[] buffer;
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			// Truong hop tap tin tai vi tri id khong co
			if (kernel->fileSystem->fileTable[id] == NULL)
			{
				printf("\nLoi Read: Tap tin khong ton tai");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			// Truong hop doc du lieu tu tap tin
			buffer = User2Kernel(virtAddr, size);						 // Chuyen du lieu tu user space sang kernel space
			oldPos = kernel->fileSystem->fileTable[id]->GetCurrentPos(); // Lay vi tri hien tai cua con tro file

			if (kernel->fileSystem->fileTable[id]->Read(buffer, size) > 0)
			{
				newPos = kernel->fileSystem->fileTable[id]->GetCurrentPos(); // Lay vi tri hien tai cua con tro file sau khi doc
				realSize = newPos - oldPos;									 // So byte doc duoc se = vi tri moi - vi tri cu
				kernel2user(virtAddr, realSize, buffer);					 // Chuyen ket qua tro lai user space
				kernel->machine->WriteRegister(2, realSize);				 // Tra ve so byte doc thuc su
				delete[] buffer;
				IncreaseCounter();
				return;
			}
			else // Truong hop file rong hoac da doc toi cuoi file
			{
				DEBUG(dbgSys, "Tap tin khong con du lieu!!\n");
				kernel->machine->WriteRegister(2, -2);
			}
			delete[] buffer;
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Write:
		{
			// Cu phap: int Write(char* buffer, int size, OpenFileId id);
			// Input: Buffer la chuoi muon ghi ra file, size la kich thuoc ma ta muon ghi, id dai dien cho file muon ghi
			// Output: tra ve -1 neu that bai, So byte ghi duoc neu thanh cong
			int realSize = 0;								  // So du lieu ghi thuc su
			int virtAddr = kernel->machine->ReadRegister(4);  // Ghi vi tri cua buffer trong thanh ghi r4
			int size = kernel->machine->ReadRegister(5);	  // Ghi kich thuoc chuoi yeu cau trong thanh ghi r5
			OpenFileId id = kernel->machine->ReadRegister(6); // Ghi OpenFileID id tu thanh ghi r6
			char *buffer;
			int oldPos = 0;
			int newPos = 0;
			// Kiem tra id cua file phai nam trong bang mo ta file
			if (id < 0 || id > 9)
			{
				printf("\nLoi Write: Khong the truy cap file vi id = %d nam ngoai bang mo ta.\n", id);
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			// Truong hop doc du lieu tu Ouput console thi loi
			if (id == InputConsoleIn)
			{
				printf("\nKhong the ghi du lieu vao STDIN (console input)\n");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			//Truong hop ghi du lieu STDOUT tu output console
			if (id == OutputConsoleOut)
			{
				buffer = User2Kernel(virtAddr, size);
				size = GetSizeOfString(buffer);
				int i = 0;
				while (buffer[i] != 0)
				{
					kernel->synchConsoleOut->PutChar(buffer[i]); //In tung ki tu ra man hinh
					i++;
				}
				kernel->machine->WriteRegister(2, i);
				delete[] buffer;
				IncreaseCounter();
				return;
			}
			// Truong hop file voi id nay khong ton tai
			if (kernel->fileSystem->fileTable[id] == NULL)
			{
				printf("\nLoi Write: Tap tin id = %d khong ton tai", id);
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			// Truong hop ghi du lieu vao file chi doc
			if (kernel->fileSystem->fileTable[id]->type == OnlyRead)
			{
				printf("\nLoi Write: Khong the ghi du lieu vao tap tin chi doc");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			buffer = User2Kernel(virtAddr, size);
			// De ghi chinh xac chuoi buffer thi ta can tinh lai kich thuoc cua no (buffer)
			size = GetSizeOfString(buffer);
			// Truoc khi ghi ta phai danh dau lai vi tri hien tai vi sau khi ghi
			oldPos = kernel->fileSystem->fileTable[id]->GetCurrentPos();

			if (kernel->fileSystem->fileTable[id]->Write(buffer, size) > 0)
			{
				// Do ta de ham Write vao dieu kien if nen can co cach khac de biet so byte da ghi
				// Ta se lay vi tri hien tai cua con tro file (vi tri sau khi ghi)
				newPos = kernel->fileSystem->fileTable[id]->GetCurrentPos();
				// So byte ghi thuc su chinh la khoang cach tu vi tri cu toi vi tri moi
				realSize = newPos - oldPos;
				// Sau khi ghi xong thi ta phai tra buffer ve lai user space
				kernel2user(virtAddr, realSize, buffer);
				kernel->machine->WriteRegister(2, realSize);
				
				delete[] buffer;
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			else // Truong hop khong ghi duoc gi vao file
				kernel->machine->WriteRegister(2, -2);
			delete buffer;
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_Exec:
		{
			// Cu phap: SpaceId Exec(char* process);
			// Input: ten chuong trinh (tien trinh) muon exec (vi du chuong trinh sinhvien)
			// Output: tra ve -1 neu that bai, tra ve spaceID dai dien cho chuong trinh vua moi exec
			int maxLength = 30;
			int virtAddr = kernel->machine->ReadRegister(4);
			char *name = User2Kernel(virtAddr, maxLength);

			if (name == NULL) // Truong hop name khong hop le
			{
				printf("Loi Exec: Khong du bo nho!!\n");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			OpenFile *file = kernel->fileSystem->Open(name);
			if (file == NULL) // Truong hop khong mo duoc file
			{
				printf("Loi Exec: Khong the mo duoc file!!\n");
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}

			delete file;
			// Truong hop nay da mo duoc file
			int result = kernel->pTab->ExecUpdate(name);
			kernel->machine->WriteRegister(2, result);
			delete[] name;

			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Join:
		{
			// Cu phap: int Join(SpaceId id);
			// Input: id dai dien cho ten chuong trinh (tien trinh) muon join (vi du chuong trinh sinhvien)
			// Output: tra ve -1 neu that bai, tra ve 0 neu chuong trinh join thanh cong
			SpaceId id = kernel->machine->ReadRegister(4); // Doc id tu thanh ghi R4

			int ec = kernel->pTab->JoinUpdate(id);
			kernel->machine->WriteRegister(2, ec);

			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Exit:
		{
			// Cu phap: void Exit(int exitcode);
			// Input: exitcode dai dien cho trang thai hoan thanh cua chuong trinh vua join
			// Output: Khong co
			int exitStatus = kernel->machine->ReadRegister(4);

			// Truong hop exitStatus != 0 nghia la chuong trinh bi loi
			if (exitStatus != 0)
			{
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			// Truong hop chuong trinh hoan thanh thanh cong
			// Goi ham cap nhat exit
			int result = kernel->pTab->ExitUpdate(exitStatus);
			// Giai phong chuong trinh khi ket thuc ham
			kernel->currentThread->FreeSpace();
			kernel->currentThread->Finish();

			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_CreateSemaphore:
		{
			// Cu phap: int CreateSemaphore(char* name, int semval);
			// Input: name dai dien cho ten semaphore muon tao, semval la so luong semaphore nay
			// Output: tra ve -1 neu that bai, 0 neu thanh cong
			int fname_size = 32;
			//Read name address
			int nameAddr = kernel->machine->ReadRegister(4);

			//Read semval
			int semval = kernel->machine->ReadRegister(5);

			char *name = User2Kernel(nameAddr, fname_size + 1);

			// Truong hop ten semaphore la rong
			if (name == NULL)
			{
				DEBUG('a', "\nNot enough memory in System");
				printf("\nNot enough memory in System");
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
			}
			// Neu name hop le thi ta goi ham Create cua lop Stable de giup ta tao semphore
			int res = kernel->semTab->Create(name, semval);
			// Truong hop ham Create khong tao duoc semaphore
			if (res == -1)
			{
				DEBUG('a', "Loi CreateSemaphore: Khong the khoi tao semaphore!!\n");
				printf("Loi CreateSemaphore: Khong the khoi tao semaphore %s!!\n", name);
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
			}
			// In ra thong bao la semaphore co ten name duoc tao thanh cong
			printf("CreateSemaphore: Tao semaphore %s thanh cong\n", name);
			delete[] name;
			kernel->machine->WriteRegister(2, res);

			IncreaseCounter();
			return;
		}

		case SC_Wait:
		{
			// Cu phap: int Wait(char* name);
			// Input: name dai dien cho ten cua semaphore muon wait
			// Output: tra ve -1 neu that bai, 0 neu thanh cong
			int fname_size = 32;
			// Read name from r4
			int nameAddr = kernel->machine->ReadRegister(4);
			// Read name from kernel
			char *name = User2Kernel(nameAddr, fname_size);

			// Check name is available
			if (name == NULL)
			{
				DEBUG('a', "\nNot enough memory in System");
				printf("\nNot enough memory in System");
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
			}

			// Check name in Stab
			int check = kernel->semTab->Wait(name);
			if (check == -1)
			{
				DEBUG('a', "Khong ton tai ten semaphore nay!!\n");
				printf("Loi Wait: Khong ton tai ten semaphore %s!!\n", name);
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
			}
			delete[] name;

			kernel->machine->WriteRegister(2, check);
			IncreaseCounter();
			return;
		}

		case SC_Signal:
		{
			// Cu phap: int Signal(char *name);
			// Input: name dai dien cho ten semaphore muon signal
			// Output: tra ve -1 neu that bai, 0 neu thanh cong
			int fname_size = 32;
			// Read name from r4
			int nameAddr = kernel->machine->ReadRegister(4);
			// Read name from kernel
			char *name = User2Kernel(nameAddr, fname_size + 1);

			// Check name is available
			if (name == NULL)
			{
				DEBUG('a', "\nNot enough memory in System");
				printf("\nNot enough memory in System");
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
			}

			// Check name in Stab
			int check = kernel->semTab->Signal(name);
			if (check == -1)
			{
				DEBUG('a', "\nKhong ton tai ten semaphore nay!");
				printf("Loi Signal: Khong ton tai semaphore %s!!\n", name);
				kernel->machine->WriteRegister(2, -1);
				delete[] name;
				IncreaseCounter();
				return;
			}
			delete[] name;

			kernel->machine->WriteRegister(2, check);
			IncreaseCounter();
			return;
		}

		case SC_Seek:
		{
			// Cu phap int Seek(int position, int id)
			//** Input: position (vi tri can seek toi), id (file id cua tap tin can seek)
			//** Output: -1 neu that bai, 0 neu thanh cong

			int position = kernel->machine->ReadRegister(4);  // Doc vi tri can tro toi
			OpenFileId id = kernel->machine->ReadRegister(5); // Doc file id

			//TODO: Kiem tra id co hop le khong
			if (id < 0 || id > 9) // id hop le: 0 -> 9
			{
				printf("\nLoi seek: id = %d nam ngoai pham vi hop le!!\n", id);
				kernel->machine->WriteRegister(2, -1);

				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			//TODO: Kiem tra voi id nay co mo file duoc khong
			if (kernel->fileSystem->fileTable[id] == NULL)
			{
				printf("\nLoi seek: file co id = %d khong ton tai!!\n", id);
				kernel->machine->WriteRegister(2, -1);

				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			//TODO: Kiem tra position co hop le
			if (position == -1) // Vi tri nay thi gan vao cuoi file
				position = kernel->fileSystem->fileTable[id]->Length();
			else if (position < 0 || position > kernel->fileSystem->fileTable[id]->Length()) // Vị trí hiện tại không thể seek tới được
			{
				printf("\nLoi seek: Vi tri %d nam ngoai pham vi kich thuoc cua tap tin co id = %d", position, id);
				kernel->machine->WriteRegister(2, -1);

				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			else // Truong hop ma position co the seek toi duoc
			{
				kernel->fileSystem->fileTable[id]->Seek(position);
				kernel->machine->WriteRegister(2, position); // Tra ve position khi thanh cong
			}
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_GetProcessID:
		{
			//** Input: void (không co dau vao)
			//** Output: process ID hien tai

			kernel->machine->WriteRegister(2, kernel->currentThread->processID);

			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_GetFileLength:
		{
			//** Input: file ID (id cua file dang mo)
			//** Output: -1 neu that bai, kich thuoc file neu thanh cong
			char tempChar;
			int fileLength = 0;
			int fid = (int)kernel->machine->ReadRegister(4); // Doc id cua file tu thanh ghi r4

			// Do bang fileTable chi quan ly 10 file (bao gom ca STDIN va STDOUT)
			if (fid < 2 || fid > 9)
			{
				printf("Loi getFileLength: id = %d nam ngoai bang fileTable!!\n", fid);
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			//TODO: Kiem tra file voi fid co ton tai
			if (kernel->fileSystem->fileTable[fid] == NULL)
			{
				printf("Loi getFileLength: file nay khong ton tai!!\n");
				IncreaseCounter();
				return;
				ASSERTNOTREACHED();
				break;
			}
			// Truoc khi tinh do dai file can luu lai con tro hien tai
			int currentPosition = kernel->fileSystem->fileTable[fid]->GetCurrentPos();
			// Ta phai dua con tro ve 0 de bat dau duyet file
			kernel->fileSystem->fileTable[fid]->Seek(0);

			/* Do ta khong biet chinh xac do dai cua file nen
			* doc tung ki tu co trong file vao tempChar
			* cap nhat so luong vao fileLength
			* qua trinh se dung khi ham Read khong tra ve 1 nua
			*/
			while (kernel->fileSystem->fileTable[fid]->Read(&tempChar, 1) == 1)
				++fileLength;
			// Sau khi duyet thi dat con tro ve vi tri cu
			kernel->fileSystem->fileTable[fid]->Seek(currentPosition);
			kernel->machine->WriteRegister(2, fileLength);
			IncreaseCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_Tell:
		{
			// Cu phap: int Tell(int id)
			// Input: id dai dien cho file muon biet vi tri hien tai
			// Output: tra ve -1 neu that bai, vi tri hien tai cua con tro neu thanh cong
			int id = kernel->machine->ReadRegister(4);
			if (id < 0 || id > 9)
			{
				DEBUG(dbgFile, "Index " << id << " is out of range.\n");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			// Kiem tra file co ton tai khong
			if (kernel->fileSystem->fileTable[id] == NULL)
			{
				DEBUG(dbgFile, "File with index "
								   << " doesnt exist.\n");
				kernel->machine->WriteRegister(2, -1);
				IncreaseCounter();
				return;
			}
			// Lay vi tri hien tai cua con tro file
			int result = kernel->fileSystem->fileTable[id]->GetCurrentPos();
			kernel->machine->WriteRegister(2, result);
			IncreaseCounter();
			return;
			break;
		}
		default:
		{
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		}
		break;
	/*Exceptions*/ //Xu ly cac truong hop exception khac trong file machine.h
	case NoException:
	{
		SysHalt();
		break;
	}
	case PageFaultException:
	{
		printf("\nNo valid translation found.\n");
		ASSERT(FALSE);
		break;
	}
	case ReadOnlyException:
	{
		printf("\nA program executed a system call.\n");
		ASSERT(FALSE);
		break;
	}
	case BusErrorException:
	{
		printf("\nA program executed a system call.\n");
		ASSERT(FALSE);
		break;
	}
	case AddressErrorException:
	{
		printf("\nA program executed a system call.\n");
		ASSERT(FALSE);
		break;
	}
	case OverflowException:
	{
		printf("\nA program executed a system call.\n");
		ASSERT(FALSE);
		break;
	}
	case IllegalInstrException:
	{
		printf("\nA program executed a system call.\n");
		ASSERT(FALSE);
		break;
	}
	case NumExceptionTypes:
	{
		printf("\nA program executed a system call.\n");
		ASSERT(FALSE);
		break;
	}
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
