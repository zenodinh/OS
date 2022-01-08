#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

class Semaphore;
class PCB
{
private:
    Semaphore *joinsem; // semaphore cho quá trình join
    Semaphore *exitsem; // semaphore cho quá trình exit
    Semaphore *multex;  // semaphore cho quá trình truy xuất đọc quyền

    int exitcode;

    char ProcessName[50]; // Ten cua tien trinh

    Thread *thread; // Tien trinh cua chuong trinh
public:
    int parentID; // ID của tiến trình cha
    PCB(int id);
    ~PCB(); // deconstructor
    // nạp chương trình có tên lưu trong biến filename và processID là pid
    int Exec(char *filename, int pid); // Tạo 1 thread mới có tên là filename và process là pid
    int GetID();                       // Trả về ProcessID của tiến trình gọi thực hiện

    void JoinWait(); // 1. Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait(); // 4. Tiến trình con kết thúc

    void JoinRelease(); // 2. Báo cho tiến trình cha thực thi tiếp
    void ExitRelease(); // 3. Cho phép tiến trình con kết thúc

    void SetExitCode(int); // Đặt exitcode của tiến trình
    int GetExitCode();     // Trả về exitcode

    void SetFileName(char *fn); // Set ten tien trinh
    char *GetFileName();      // Tra ve ten tien trinh
};

#endif