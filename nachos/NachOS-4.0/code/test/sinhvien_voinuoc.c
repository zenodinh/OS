// #include "syscall.h"

// #define MAX_STUDENTS_IN 5
// // Bai toan: n sinh vien xai 1 voi nuoc

// int main(){
//     int fileID; // Dung de luu lai id cua file sau Open
//     int SLSV; // So luong sinh vien rot nuoc
//     int i;// Dung cho ham for
//     char temp; // Bien tam
//     int SinhVien[MAX_STUDENTS_IN]; // Khai bao so vien vien toi da = 5
//     SLSV = 0;
//     // Kiem tra mo file input.txt
//     fileID = Open("input.txt", OnlyRead);
//     if(fileID == -1)
//     {
//         PrintString("Tap tin input.txt khong the mo duoc!!\n");
//         return 0;
//     }
//     //TODO: Doc so luong sinh vien 
//     while(1) {
//         if(Read(&temp, 1, fileID)== -1){
//             PrintString("Khong the doc duoc file!!\n");
//             Close(fileID);
//             return 0;
//         }
//         //TODO: Cap nhat so luong sinh vien
//         SLSV = SLSV * 10 +  ((int)temp - 48);
//     }
//     //TODO: So luong sinh vien = 1 -> 5
//     if(SLSV < 1 || SLSV > 5){
//         PrintString("So luong sinh vien vuot muc cho phep!!\n");
//         Close(fileID);
//         return 0;
//     }
//     //TODO: Tao file output.txt de xuat ket qua
//     if(Create("../test/output.txt") == -1)
//     {
//          PrintString("Khong the tao file output.txt!!\n");
//         Close(fileID); 
//         return 0;
//     }
//     //TODO: Tao 2 semaphore
//     CreateSemaphore("Main", 0);
//     CreateSemaphore("Lock", 1);

//     for(i = 0; i< SLSV; ++i)
//         SinhVien[i] = Exec("../test/sinhvien");
    
//     temp = SLSV;
//     while(temp > 0){
//         Wait("Main");
//         temp--;
//     }
//     for(i = 0; i< SLSV; ++i)
//         Join(SinhVien[i]);
//     Close(fileID);
//     return 1;
// }

