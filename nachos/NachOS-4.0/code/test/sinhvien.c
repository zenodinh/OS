#include "syscall.h"
void WriteNum(int num, int FileID)
{
	char buffer[11]; // chieu dai dai nhat cua chuoi int trong C duoi dang string
	int i;
	int length = 0;
	int tempNum = num;
	//In ra man hinh so 0
	if (num == 0)
	{
		buffer[0] = '0';
	}
	//Lay so luong chu so cua bien num
	while (tempNum > 0)
	{
		tempNum /= 10;
		++length;
	}
	// Ta se gan vi tri cuoi la length = ki tu ket thuc \0
	buffer[length] = '\0';
	for (i = length - 1; i >= 0; i--) //di tu don vi tang len
	{
		buffer[i] = num % 10 + '0';
		num /= 10;
	}
	Write(buffer, length, FileID); // ghi so num vao file
}

int main()
{
    // So lit con lai can phai rot cua sinh vien
    int LitConLai;
    // STT cua sinh vien
    int SVIndex; // <=> processID
    // Do dai cua file
    int fLength;
    // OpenfileID cua file output.txt
    int outputId;

    while (Wait("Voinuoc") == 0) // Doi mien gang trong thi tranh vao rot nuoc
    {
        // Do cac bien duoc khai bao o tren la global nen co the xay ra truong hop 
        // co tien trinh khac nhay vao thay doi gia tri cua tien trinh hien tai
        // Neu tranh duoc thi ta phai Wait de cap nhat cung nhu khoi tao cac gia tri cho bien global
        SVIndex = GetProcessID();
        LitConLai = 10;
        outputId = Open("output.txt", ReadAndWrite);
        // Thong bao la sinh vien thu SVIndex dang bat dau mien gang va thuc hien rot nuoc
        PrintString("Sinh vien ");
        PrintNum(SVIndex);
        PrintString(" bat dau rot nuoc\n");

        // Sau khi thuc hien cac buoc tren thi ta tra lai mien gang cho cac tien trinh khac
        Signal("Voinuoc");
        // Sau do ta thuc hien rot lan luot tung lit nuoc vao chai cho sinh vien hien tai
        while (LitConLai > 0)
        {
            // Phai dong mien gang de trong qua trinh rot nuoc khong co tien trinh khac xen ngang
            Wait("Voinuoc");

            --LitConLai;
            // Ghi ket qua vao file output.txt
            fLength = GetFileLength(outputId);
            Seek(fLength, outputId);
            if (fLength != 0)
                Write(", ", 2, outputId);
            WriteNum(SVIndex, outputId);
            // Sau khi rot xong thi ta tra lai voi nuoc cho tien trinh khac su dung
            Signal("Voinuoc");
        }
        // Ket thuc thi dong file output.txt 
        Close(outputId);
        // Bao hieu quay tro lai tien trinh Main
        Signal("Main");
        return 1;
    }
}