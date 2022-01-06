#include "syscall.h"

// Ta gan thu tu sinh vien = process id

/* Ghi 1 so int vao file voi cac input:
*+ number la so int
*+ fileID la id cua file ket qua can ghi vao
*/
void WriteNum(int number, int fileID)
{
    // Chuoi buufer co do dai = do dai integer, dung de in integer ra
    char buffer[11];
    // Do dai cua bien number
    int length;
    // Bien tam thoi
    int tempNum;
    // Bien dung cho ham for
    int i;
    length = 0; // Ban dau ta gan = 0
    tempNum = number;
    if (number == 0)
    {
        Write("0", 1, fileID);
        return;
    }
    // Dem do dai cua bien SVIndex
    while (tempNum != 0)
    {
        tempNum /= 10;
        ++length;
    }
    // Chuyen doi int -> char va them vao buffer
    // Duyet tu phai -> trai
    for (i = length - 1; i > -1; --i)
    {
        buffer[i] = (char)(number % 10 + 48);
        number /= 10;
    }
    // Ghi ket qua vao file co fileID
    Write(buffer, length, fileID);
}

int GetSolit(char *filename, int index)
{
    int litId, count, result, solit;
    char c_read;
    count = solit = 0;
    litId = Open(filename, OnlyRead);
    // Nhay ve vi tri dau
    Seek(0, litId);
    // Nhay toi vi tri can doc ung voi SVIndex
    while (count < index - 1)
    {
        result = Read(&c_read, 1, litId);
        if (result == 1)
        {
            if (c_read == '\n')
                ++count;
        }
        else
        {
            PrintString("Di chuyen con tro file trong ham GetSolit khong thanh cong\n");
        }
    }

    while (1) // Doc vao gia tra cua lit con lai
    {
        result = Read(&c_read, 1, litId);
        if (result == 1 && c_read != ' ' && c_read != '\n')
        {
            solit = solit * 10 + ((int)c_read - 48);
        }
        else
            break;
    }
    Close(litId);
    return solit;
}

void WriteSolit(char *filename, int index, int solit)
{
    int litId, count, result;
    int num[50];
    char c_read;
    int length;
    int c_num;
    int i;

    litId = Open(filename, ReadAndWrite);
    // Cap nhat so lit con lai vao file litconlai.txt
    // Nhay ve vi tri dau
    Seek(0, litId);
    // Nhay toi vi tri ung voi index
    count = 0;
    while (count < index - 1)
    {
        result = Read(&c_read, 1, litId);
        if (result == 1)
        {
            if (c_read == '\n')
                ++count;
        }
        else
        {
            PrintString("Di chuyen con tro file trong ham WriteSolit 1 khong thanh cong\n");
        }
    }
    c_num = 0;
    length = 0;
    num[length++] = solit;
    while (1) // Doc vao gia tra cua lit con lai
    {
        result = Read(&c_read, 1, litId);
        if (result == 1)
        {
            if (c_read >= '0' && c_read <= '9')
                c_num = c_num * 10 + ((int)c_read - 48);
            if (c_read == ' ' || c_read == '\n')
            {
                num[length++] = c_num;
                c_num = 0;
            }
        }
        else if (result == -2)
        {
            num[length++] = c_num;
            break;
        }
    }
    Seek(0, litId); // Nhay ve vi tri dau
    // Nhay toi vi tri ung voi index
    count = 0;
    while (count < index - 1)
    {
        result = Read(&c_read, 1, litId);
        if (result == 1)
        {
            if (c_read == '\n')
                ++count;
        }
        else
        {
            PrintString("Di chuyen con tro file trong ham WriteSolit 2 khong thanh cong\n");
        }
    }
    for (i = 0; i < length; i++)
    {
        WriteNum(num[i], litId);
        if (i == length - 1)
            break;
        if (num[i] == 10)
            Write("\n", 1, litId);
        else
            Write(" ", 1, litId);
    }
    Close(litId);
}

void main()
{
    // So lit con lai can phai rot cua sinh vien
    int LitConLai;
    // STT cua sinh vien
    int SVIndex; // <=> processID
    // Do dai cua file
    int fLength;
    // OpenfileID cua file
    int outputId;
    int solit;
    int result;

    // Wait("lock");
    SVIndex = GetProcessID();
    solit = 10;
    outputId = Open("output.txt", ReadAndWrite);

    while (solit > 0)
    {
        Wait("lock");
        --solit;
        // Ghi ket qua vao file output.txt

        fLength = GetFileLength(outputId);
        Seek(fLength, outputId);
        if (fLength != 0)
            Write(", ", 2, outputId);
        WriteNum(SVIndex, outputId);

        Signal("lock");
    }
    Close(outputId);
    Signal("main");
    return;
}