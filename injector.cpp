// cl injector.cpp /Fe:injector.exe /link user32.lib kernel32.lib
// injector.exe <PID> <full_path_to_your_DLL>
#include <windows.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: injector.exe <PID> <PathToDLL>\n";
        return 1;
    }

    DWORD pid = std::stoul(argv[1]);
    const char* dllPath = argv[2];

    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);

    if (!hProcess) {
        std::cerr << "Failed to open target process.\n";
        return 1;
    }

    LPVOID alloc = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1,
                                  MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!alloc) {
        std::cerr << "Failed to allocate memory in target process.\n";
        CloseHandle(hProcess);
        return 1;
    }

    WriteProcessMemory(hProcess, alloc, dllPath, strlen(dllPath) + 1, nullptr);

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"),
        alloc, 0, nullptr);

    if (!hThread) {
        std::cerr << "Failed to create remote thread.\n";
        VirtualFreeEx(hProcess, alloc, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    std::cout << "DLL injected successfully.\n";

    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, alloc, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return 0;
}
