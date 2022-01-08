#include "syscall.h"
#define MAX_STUDENTS 5
// Bai toan: n sinh vien xai 1 voi nuoc

int ReadSLSV(int FileID)
{
    int result = 0;
    char ch;
    while (1)
    {
        if (Read(&ch, 1, FileID) == 1)
        {
            if (ch >= '0' && ch <= '9')
                result = result * 10 + (ch - '0');
        }
        else
        {
            return result;
        }
    }
    return -1;
}

int main()
{
    int fileID;             // Dung de luu lai id cua file sau Open
    int SLSV;               // So luong sinh vien rot nuoc
    int i;                  // Dung cho ham for
    char temp;              // Bien tam
    int exec[MAX_STUDENTS]; // Khai bao so vien vien toi da = 5
    int join[MAX_STUDENTS];
    PrintChar('\n');
    // Kiem tra mo file input.txt
    fileID = Open("input.txt", OnlyRead);
    if (fileID == -1)
    {
        PrintString("Tap tin input.txt khong the mo duoc!!\n");
        return 0;
    }
    //TODO: Doc so luong sinh vien
    SLSV = ReadSLSV(fileID);

    //TODO: So luong sinh vien = 1 -> 5
    if (SLSV < 1 || SLSV > MAX_STUDENTS)
    {
        Close(fileID); // Dong file input.txt
        PrintString("So luong sinh vien vuot muc cho phep!!\n");
        return 0;
    }
    //TODO: Tao file output.txt de xuat ket qua
    if (Create("output.txt") == -1)
    {
        Close(fileID); // Dong file input.txt
        PrintString("Khong the tao file output.txt!!\n");
        return 0;
    }
    // TODO: Tao 2 semaphore
    CreateSemaphore("Main", 0); // Main dai dien cho tien trinh cha la sinhvien_voinuoc
    CreateSemaphore("Voinuoc", 1); // Voinuoc dai dien cho mien gang voi nuoc ma cac sinh vien
                                   // la cac chuong trinh muon tranh nhau de su dung

    for (i = 0; i < SLSV; ++i)
        exec[i] = Exec("sinhvien"); // Ta thuc hien chay cac chuong trinh hay cho sinh vien tranh nhau rot nuoc

    temp = SLSV;
    // Ham while tuong trung cho viec cac sinh vien lan luot hoan tat viec rot nuoc
    while (temp > 0)
    {
        Wait("Main");
        --temp;
    }
    // Sau khi sinh vien rot nuoc xong thi ta se goi join de cho tien trinh sinh vien quay lai ve tien trinh main
    for (i = 0; i < SLSV; ++i)
        join[i] = Join(exec[i]);
    // Va cuoi cung la goi exit de thoat tien trinh sinh vien da join
    for (i = 0; i < SLSV; ++i)
        Exit(join[i]);
        
    Close(fileID); // Dong file input.txt
    return 1;
}
