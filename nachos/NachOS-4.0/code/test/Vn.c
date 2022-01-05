#include "syscall.h"

void main()
{
	// File vn_input.txt dung de doc dung tich nuoc cua cac sinh vien
	// File vn_output.txt dung de ghi voi nuoc nao dang duoc su dung
	SpaceId vnInput, vnOutput; // Bien id cho cua file voinuoc.txt và result.txt
	char c_readFile;		   // Bien ki tu luu ki tu doc tu file
	int voi1, voi2;			   // Voi nuoc 1, voi nuoc 2
	int DungTichNuoc;		   // Dung tich nuoc cua sinh vien
	int outputSuccess;		   // Bien co luu dau hieu doc xong file vn_output

	//-----------------------------------------------------------
	voi1 = 0;
	voi2 = 0;
	DungTichNuoc = 0;
	outputSuccess = 0;
	// Xu ly voi nuoc
	while (1)
	{
		Wait("m_vn");
		// Mo file result.txt de ghi voi nao su dung
		vnOutput = Open("../test/vn_output.txt", ReadAndWrite);
		if (vnOutput == -1) // Neu doc khong duoc
		{
			Signal("sinhvien"); // Cho cac sinh vien khac vao mien gang
			return;
		}
		while (1)
		{
			Wait("voinuoc");
			c_readFile = 0;
			// Mo file vn_input.txt de doc dung tich nuoc cua cac sinh vien
			vnInput = Open("../test/vn_input.txt", OnlyRead);
			if (vnInput == -1) // Neu doc khong duoc
			{
				Close(vnOutput);
				Signal("sinhvien");
				return;
			}
			while (1)
			{
				if (Read(&c_readFile, 1, vnInput) == -2) // Da doc het du lieu
				{
					Close(vnInput);
					break;
				}
				if (c_readFile != '*')
					DungTichNuoc = DungTichNuoc * 10 + (c_readFile - 48);
				else
				{
					outputSuccess = 1;
					Close(vnInput);
					break;
				}
			}
			if (DungTichNuoc != 0)
			{
				// Dung voi 1
				if (voi1 <= voi2)
				{
					voi1 += DungTichNuoc;
					Write("1", 1, vnOutput);
				}
				else // Dung voi 2
				{
					voi2 += DungTichNuoc;
					Write("2", 1, vnOutput);
				}
			}
			if (outputSuccess == 1)
			{
				voi1 = voi2 = 0;
				Close(vnOutput);
				Signal("sinhvien");
				break;
			}
			Signal("sinhvien");
		}
	}
}