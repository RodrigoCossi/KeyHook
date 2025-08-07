#include <windows.h>
#include <iostream>

int main() {
    // Target process ID (replace with real PID of notepad or another process)
    DWORD pid = 1234;  // <- Use Task Manager or EnumProcesses to get this

    // DLL to inject
    const char* dllPath = "C:\\Path\\To\\Your\\log.dll";

    // Open target process with required permissions
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        std::cerr << "Failed to open target process\n";
        return 1;
    }

    // Allocate space in target for DLL path
    LPVOID allocMem = VirtualAllocEx(hProcess, nullptr, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!allocMem) {
        std::cerr << "Memory allocation failed\n";
        CloseHandle(hProcess);
        return 1;
    }

    // Write DLL path into allocated memory
    WriteProcessMemory(hProcess, allocMem, dllPath, strlen(dllPath) + 1, nullptr);

    // Get address of LoadLibraryA (used to load the DLL)
    LPTHREAD_START_ROUTINE loadLib = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    // Create a thread in the target process to call LoadLibraryA with our DLL
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, loadLib, allocMem, 0, nullptr);
    if (!hThread) {
        std::cerr << "Thread creation failed\n";
        VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    std::cout << "DLL injected successfully.\n";

    // Cleanup
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return 0;
}
