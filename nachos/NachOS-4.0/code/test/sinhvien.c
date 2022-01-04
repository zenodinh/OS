// #include "syscall.h"

// // Ta gan thu tu sinh vien = process id

// /* Ghi STT cua sinh vien hien tai voi cac input:
// *+ SVIndex la STT cua sinh vien hien tai
// *+ fileID la id cua file ket qua can ghi vao
// */
// void WriteSV(int SVIndex, int fileID)
// {
//     // Chuoi buufer co do dai = do dai integer, dung de in integer ra
//     char buffer[11];
//     // Do dai cua bien SVIndex
//     int SVLength;
//     // Bien tam thoi
//     int tempNum;
//     // Bien dung cho ham for
//     int i;
//     SVLength = 0;      // Ban dau ta gan = 0
//     tempNum = SVIndex;

//     // Dem do dai cua bien SVIndex
//     while (tempNum != 0)
//     {
//         tempNum /= 10;
//         ++SVLength;
//     }
//     // Chuyen doi int -> char va them vao buffer
//     // Duyet tu phai -> trai
//     for (i = SVLength - 1; i > -1; --i)
//     {
//         buffer[i] = (char)(SVIndex % 10 + 48);
//         SVIndex /= 10;
//     }
//     // Ghi ket qua vao file co fileID
//     Write(buffer, SVLength, fileID);
// }

// int main()
// {
//     // So lit con lai can phai rot cua sinh vien
//     int LitConLai;
//     // STT cua sinh vien
//     int SVIndex; // <=> processID
//     // Do dai cua file
//     int fLength;
//     // OpenfileID cua file
//     int fileID;
//     // Ten tap tin can xuat ket qua
//     char filename[11];
//     LitConLai = 10; // Ban dau sinh vien chua rot nuoc
//     SVIndex = GetProcessID();
//     fileID = Open(filename, 0);
//     if (fileID == -1)
//     {
//         PrintString("Khong mo duoc file output.txt!!\n");
//         return 0;
//     }
//     // Truong hop seek thanh cong
//     while (LitConLai > 0)
//     {
//         Wait("Lock");
//         --LitConLai;
//         fLength = GetFileLength(fileID);
//         if (fLength > 0)
//         {
//             if (Seek(fLength, fileID) == -1)
//             {
//                 PrintString("Loi khong the seek toi vi tri moi!!\n");
//                 return 0;
//             }
//             Write(", ", 5, fileID);
//             WriteSV(SVIndex, fileID);
//         }
//         Signal("Lock");
//     }
//     Close(fileID);
//     Signal("Main");
//     return 0;
// }