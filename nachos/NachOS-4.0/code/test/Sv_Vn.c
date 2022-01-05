#include "syscall.h"

#define MAX_LENGTH 32

int main()
{
	// KHAI BAO
	int f_Success;										 // Bien co dung de kiem tra thanh cong
	SpaceId si_input, si_output, si_sinhvien, si_result; // Bien id cho file
	int SLTD;											 // So luong thoi diem xem xet (dong dau tien trong file)
	char c_readFile;									 // Bien ki tu luu ki tu doc tu file

	//-----------------------------------------------------------
	// Khoi tao 4 Semaphore de quan ly 3 tien trinh
	f_Success = CreateSemaphore("main", 0);
	if (f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("sinhvien", 0);
	if (f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("voinuoc", 0);
	if (f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("m_vn", 0);
	if (f_Success == -1)
		return 1;

	// Tao file output.txt de ghi ket qua cuoi cung
	f_Success = CreateFile("output.txt");
	if (f_Success == -1)
		return 1;

	// Mo file input.txt chi de doc
	si_input = Open("input.txt", OnlyRead);
	if (si_input == -1)
		return 1;

	// Mo file output.txt de doc va ghi
	si_output = Open("output.txt", ReadAndWrite);
	if (si_output == -1)
	{
		Close(si_input);
		return 1;
	}

	// Doc so luong thoi diem xet o file input.txt
	//**** Thuc hien xong doan lenh duoi thi con tro file o input.txt o dong 1
	SLTD = 0;
	while (1)
	{

		if (Read(&c_readFile, 1, si_input) == 1)
		{
			if (c_readFile >= '0' && c_readFile <= '9')
				SLTD = SLTD * 10 + (c_readFile - 48);
		}
		else
			break;
	}

	// Goi thuc thi tien trinh sinhvien trong file Sv.c
	f_Success = Exec("../test/Sv");
	if (f_Success == -1)
	{
		Close(si_input);
		Close(si_output);
		return 1;
	}

	// Goi thuc thi tien trinh voinuoc trong file Vn.c
	f_Success = Exec("../test/Vn");
	if (f_Success == -1)
	{
		Close(si_input);
		Close(si_output);
		return 1;
	}

	// Thuc hien xu ly khi nao het thoi diem xet thi thoi
	while (SLTD--)
	{
		// Tao file sinhvien.txt
		f_Success = CreateFile("sinhvien.txt");
		if (f_Success == -1)
		{
			Close(si_input);
			Close(si_output);
			return 1;
		}

		// Mo file sinhvien.txt de ghi tung dong sinhvien tu file input.txt
		si_sinhvien = Open("sinhvien.txt", 0);
		if (si_sinhvien == -1)
		{
			Close(si_input);
			Close(si_output);
			return 1;
		}
		while (1)
		{
			if (Read(&c_readFile, 1, si_input) == 1) // Doc toi cuoi file
			{
				if (c_readFile != '\n')
					Write(&c_readFile, 1, si_sinhvien);
			}
			else
				break;
		}
		// Dong file sinhvien.txt lai
		Close(si_sinhvien);

		// Goi tien trinh sinhvien hoat dong
		Signal("sinhvien");

		// Tien trinh chinh phai cho
		Wait("main");

		// Thuc hien doc file tu vn_output va ghi ket qua vao file output.txt
		si_result = Open("vn_output.txt", OnlyRead);
		if (si_result == -1)
		{
			Close(si_input);
			Close(si_output);
			return 0;
		}

		PrintString("\n Lan thu: ");
		PrintInt(SLTD);
		PrintString("\n");
		// Doc cac voi vao output.txt
		while (1)
		{
			if (Read(&c_readFile, 1, si_result) < 1)
			{
				Write("\r\n", 2, si_output);
				Close(si_result);
				Signal("m_vn");
				break;
			}
			Write(&c_readFile, 1, si_output);
			Write(" ", 1, si_output);
		}
	}
	Close(si_input);
	Close(si_output);
	return 0;
}
