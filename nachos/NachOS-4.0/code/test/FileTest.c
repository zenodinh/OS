#include "syscall.h"

int main()
{
    char filename[100];
    char writeString[100];
    char readString[100];
    int id;
    int resultCode;
    int type;
    PrintString("Vui long nhap ten file\n");
    PrintString("Vi du: filename.txt\n");
    PrintString("Vui long nhap ten file create (do dai toi da la 50): ");
    ReadString(filename, 100);
    resultCode = Create(filename);
    if (resultCode == 0)
    {
        PrintString("\nVui long nhap kieu file muon open (0: binh thuong, 1: chi doc): ");
        type = ReadNum();
        id = Open(filename, type);
        if (id > 1)
        {
            PrintString("Nhap chuoi 1 muon them vao file: ");
            ReadString(writeString, 100);
            resultCode = Write(writeString, 100, id);
            PrintString("Nhap chuoi 2 muon them vao file: ");
            ReadString(writeString, 100);
            Seek(resultCode, id);
            resultCode = Write(writeString, 100, id);
            resultCode = Close(id);
            if (resultCode == 0)
                PrintString("\nClose file thanh cong\n");
            else
                PrintString("\nClose file that bai\n");
        }
        id = Open(filename, type);
        resultCode = Read(readString, 100, id);
        PrintString("\nChuoi doc duoc tu file la: ");
        PrintString(readString);
        PrintString("\n");
        resultCode = Close(id);
        if (resultCode == 0)
            PrintString("\nClose file thanh cong\n");
        else
            PrintString("\nClose file that bai\n");
    }
    Halt();
    /* not reached */
}