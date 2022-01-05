#include "syscall.h"

void main()
{
	int IsSuccess;				  // Bien dung de kiem tra ham thuc thi co thanh cong hay khong
	SpaceId si_sinhvien, vnInput; // Bien id cho tien trinh sinh vien va voi nuoc
	char c_readFile;			  // Bien ki tu luu ki tu doc tu file
	int voinuocJump;			  // Bien dung de nhay den tien trinh voinuoc
	int mainJump;				  // Bien dung de nhay den tien trinh main
	int fileLength;				  // Luu do dai file
	int filePointer;			  // Luu con tro file

	//-----------------------------------------------------------
	Signal("m_vn");

	while (1)
	{
		fileLength = 0;
		Wait("sinhvien");
		// Tao file vn_output.txt de ghi voi nao su dung
		IsSuccess = Create("../test/vn_output.txt");
		if (IsSuccess == -1)
		{
			Signal("main"); // tro ve tien trinh chinh
			return;
		}

		// Mo file sinhvien.txt len de doc
		si_sinhvien = Open("../test/sinhvien.txt", 1);
		if (si_sinhvien == -1)
		{
			Signal("main"); // tro ve tien trinh chinh
			return;
		}

		fileLength = Seek(-1, si_sinhvien); // Nhay toi vi tri cuoi
		Seek(0, si_sinhvien);
		filePointer = 0;

		// Tao file vn_input.txt
		IsSuccess = Create("../test/vn_input.txt");
		if (IsSuccess == -1)
		{
			Close(si_sinhvien);
			Signal("main"); // tro ve tien trinh chinh
			return;
		}

		// Mo file vn_input.txt de ghi tung dung tich nuoc cua sinhvien
		vnInput = Open("../test/vn_input.txt", 0);
		if (vnInput == -1)
		{
			Close(si_sinhvien);
			Signal("main"); // tro ve tien trinh chinh
			return;
		}

		// Ghi dung tich vao file voinuoc.txt tu file sinhvien.txt
		while (filePointer < fileLength)
		{
			voinuocJump = 0;
			Read(&c_readFile, 1, si_sinhvien);
			if (c_readFile != ' ')
				Write(&c_readFile, 1, vnInput);
			else
				voinuocJump = 1;
			if (filePointer == fileLength - 1)
			{
				Write("*", 1, vnInput);
				voinuocJump = 1;
			}

			if (voinuocJump == 1)
			{
				Close(vnInput);
				Signal("voinuoc");
				// Dung chuong trinh sinhvien lai de voinuoc thuc thi
				Wait("sinhvien");

				// Mo file vn_input.txt de ghi tung dung tich nuoc cua sinhvien
				vnInput = Open("../test/vn_input.txt", 0);
				if (vnInput == -1)
				{
					Close(si_sinhvien);
					Signal("main"); // tro ve tien trinh chinh
					return;
				}
			}
			filePointer++;
		}
		// Ket thuc tien trinh sinhvien va voinuoc quay lai ham SvVn
		Signal("main");
		PrintString("Chay sinhvien duoc\n");
	}
	// Quay lai ham Svvn
}
