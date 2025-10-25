# KeyHook - Exploiting DLL Injections for Keylogging

</br>

> ⚠️ **Disclaimer:**
> <h3>This project is for educational and ethical purposes only — do not use its code or concepts in unauthorized environments. Keylogging or injecting code into other processes is illegal if used maliciously.</h3>

</br>

## Intro about DLLs:

DLL injection is a technique used to run code within the address space of another process by forcing it to load a dynamic-link library (DLL) - a shared module used natively in Windows systems to control essential resources and functionalities. ([Microsoft Documentation](https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-libraries)).

This method allows the injected code to manipulate the target process's behavior, which can be used for both legitimate and malicious purposes.

The injected DLL can perform various actions, from altering the process's behavior to stealing sensitive information. It's a technique used in debugging, game modding, malware analysis, but also in data theft, bypassing protections, or functionality hijacking. 

</br>

## 🚀 Features

- **System-wide Key Capture**: Low-level keyboard hook captures all keystrokes across the entire system
- **DLL Injection**: Injects keylogger into target processes for stealth operation
- **Character Conversion**: Converts virtual key codes to readable characters
- **Special Key Support**: Handles Shift, Caps Lock, punctuation, and function keys
- **Real-time Logging**: Immediate keystroke logging to file
- **Process Persistence**: Runs as long as the host process is active
- **x64 Architecture**: Compiled for 64-bit Windows systems

## 📁 Project Structure

```
KeyHook/
├── keyboardhook.cpp        # Main keylogger DLL with keyboard hook
├── injector.cpp            # DLL injector utility
├── build.bat               # Automated build script
├── built/                  # Generated compiled files
│   ├── keylogger.dll       # Main keylogger DLL
│   ├── injector.exe        # DLL injection tool
├── log.txt                 # Keystroke log file (generated)
└── README.md               # This documentation
```

## 🔧 Prerequisites

- **Windows 10/11** (64-bit)
- **Visual Studio 2019/2022** or **Visual Studio Build Tools**
- **Administrator privileges** (recommended for injection)

### Visual Studio Installation Options:
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/community/) (Free)
- [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) (Minimal)

## 🛠️ Build Instructions

### Automated Build (Recommended)

1. **Clone/Download** the project to your local machine
2. **Open any Command Prompt or PowerShell** (regular window, not elevated)
3. **Navigate** to the project directory:
   ```powershell
   cd C:\path\to\KeyHook
   ```
4. **Run the build script**:
   ```powershell
   .\build.bat
   ```

The build script will:
- Auto-detect Visual Studio installation
- Set up the x64 compilation environment
- Compile all components
- Place executables in the `built/` directory

### Manual Build (Alternative)

If you prefer manual compilation:

1. **Open "x64 Native Tools Command Prompt for VS"**
2. **Navigate to project directory**
3. **Compile individual components**:
   ```cmd
   cl /LD /EHsc /Fekeylogger.dll keyboardhook.cpp user32.lib kernel32.lib
   cl /EHsc /Feinjector.exe injector.cpp user32.lib kernel32.lib
   ```

## 🎯 Usage

### Step 1: Identify Target Process

Get the Process ID (PID) of your target process. For system-wide monitoring, use `explorer.exe`:

```powershell
Get-Process explorer | Select-Object Id,ProcessName
```

**Output example:**
```
   Id ProcessName
   -- -----------
12345 explorer
```

### Step 2: Inject Keylogger

Inject the keylogger DLL into the target process:

```powershell
.\built\injector.exe 12345 "C:\full\path\to\KeyHook\built\keylogger.dll"
```

**Expected output:**
```
DLL injected successfully.
```

### Step 3: Monitor Keystrokes

The keylogger will create `log.txt` and capture all keystrokes:

```powershell
# View recent keystrokes
Get-Content log.txt -Tail 20

# Monitor in real-time
Get-Content log.txt -Wait -Tail 5
```

### Step 4: Stop Keylogger

To stop the keylogger, terminate the host process:

```powershell
# For explorer.exe (will restart automatically)
taskkill /f /im explorer.exe
Start-Process explorer.exe

# For other processes
taskkill /f /im target_process.exe
```

## 📋 Components Explained

### 1. Keylogger DLL (`keyboardhook.cpp`)

The main component implementing:
- **Low-Level Keyboard Hook** (`WH_KEYBOARD_LL`)
- **Virtual Key to Character Conversion**
- **File Logging with Absolute Paths**
- **Shift/Caps Lock State Detection**

**Key Features:**
- Captures letters (a-z, A-Z) with proper case
- Numbers (0-9) and symbols with Shift
- Special keys ([ENTER], [BACKSPACE], [F1-F12], etc.)
- Punctuation and symbols

### 2. DLL Injector (`injector.cpp`)

Utility for injecting DLLs into target processes:
- **Process Memory Allocation**
- **Remote Thread Creation**
- **LoadLibrary Injection Technique**
- **Error Handling and Cleanup**


## 🔍 Technical Details

### Architecture
- **Target**: x64 Windows processes
- **Hook Type**: Low-level keyboard hook (WH_KEYBOARD_LL)
- **Injection Method**: Classic DLL injection via CreateRemoteThread
- **Persistence**: Runs within host process context

### Key Conversion Logic
```cpp
// Example: Virtual Key Code → Character
VK_A (0x41) → 'a' or 'A' (depending on Shift/Caps)
VK_1 (0x31) → '1' or '!' (depending on Shift)
VK_SPACE (0x20) → ' '
VK_RETURN (0x0D) → '\n'
```

### File Logging
- **Location**: Project directory (`log.txt`)
- **Format**: Plain text, real-time appending
- **Path**: Absolute path to handle working directory changes

## ⚙️ How DLL Injection Works

1. **Find the Target Process**
   <br> Use Windows APIs like `OpenProcess()` to get a handle to the process you want to inject into.

2. **Allocate Memory**
   <br> Allocate memory inside the target process using `VirtualAllocEx()`.

3. **Write DLL Path**
   <br> Write the path to the DLL into that memory using `WriteProcessMemory()`.

4. **Create Remote Thread**
   <br> Use `CreateRemoteThread()` to run `LoadLibraryA()` inside the target process, which loads the DLL.

### 🔐 Result

The injected DLL now runs **as if it's part of the target application**, sharing its memory and privileges.

---

## 🧭 Visual Diagram: DLL Injection Flow

```
[Injector Process - injector.exe]
              │
    ┌─────────┴────────────┐
    │ Open Target Process  │   ← OpenProcess
    └─────────┬────────────┘
              │
    ┌─────────▼────────────┐
    │ Allocate Memory      │   ← VirtualAllocEx
    └─────────┬────────────┘
              │
    ┌─────────▼────────────┐
    │ Write DLL Path       │   ← WriteProcessMemory
    └─────────┬────────────┘
              │
    ┌─────────▼────────────┐
    │ Get LoadLibraryA     │   ← GetProcAddress
    └─────────┬────────────┘
              │
    ┌─────────▼────────────┐
    │ Start Remote Thread  │   ← CreateRemoteThread
    └─────────┬────────────┘
              │
    ┌─────────▼────────────┐
    │ DLL Loaded into      │
    │ Target Process       │
    └─────────┬────────────┘
              │
              ▼
    ┌───────────────────────────────┐
    │ DllMain(DLL_PROCESS_ATTACH):  │  ← Automatically called
    │                               │
    │ - DisableThreadLibraryCalls() │
    │ - CreateThread(...)           │  ← Background thread for hook
    └─────────┬─────────────────────┘
              │
    ┌─────────▼─────────────────────────┐
    │ HookThread():                     │
    │ - SetWindowsHookEx(WH_KEYBOARD_LL)│ ← Installs low-level keyboard hook
    │ - Loop: GetMessage/Dispatch       │ ← Keeps hook alive (event loop)
    └───────────────────────────────────┘
```

---

## 💥 What Could the DLL Do to Target Processes?

Once injected into a process's memory space, your DLL has **full access** to its memory, windows, UI, keyboard events, and more. Here's what it could potentially do:

---

### 🧪 1. Monitor or Manipulate Text Input

* Hook into the process's **text buffer** to log user input.
* **Automatically modify** text typed into applications in real time.

---

### 🎯 2. Hook API Calls

* Intercept system calls like `WriteFile`, `ReadFile`, `SendMessage`, etc.
* Modify or redirect the process's behavior at **runtime**.

---

### 🔍 3. Read or Write Process Memory

* Scan process memory for sensitive content (e.g., typed text, clipboard).
* **Exfiltrate** in-memory data or inject custom content.

---

### 🧨 4. Inject Code into Other Threads

* Create or hijack threads within the target process.
* Use the process as a **stealth container** to run hidden operations.

---

### � 5. Use Process as a Launchpad

* Launch additional processes or payloads from within the target.
* Communicate with external systems using the process's identity to evade detection.

---

## 🛡️ Security Considerations

### Detection Avoidance
- Runs within legitimate system processes
- No standalone process footprint
- Minimal API calls
- File logging can be customized/encrypted

### Additional Evasion (Educational):
- **Reflective DLL Loading**: Load from memory without touching disk
- **API Unhooking**: Bypass EDR hooks before injection
- **Process Hollowing**: Replace legitimate process memory
- **Signed DLLs**: Use legitimate signed libraries as carriers


### Blue Team Potential Detection methods:

| Technique | Detection Method |
|-----------|------------------|
| DLL Injection | Monitor `CreateRemoteThread`, `VirtualAllocEx` calls |
| Keyboard Hooks | Detect `SetWindowsHookEx` in unusual processes |
| Suspicious Files | Monitor writes to `log.txt`, unusual DLL loads |
| Process Behavior | Notepad/Explorer with unexpected network/file activity |
| Process Module Enumeration |scan process's memory to detect modules that are in memory but without a corresponding file path on disk|
| Hook Chain Inspection | look for unexpected modifications on critical Windows functions, like on `kernel32.dll` (API Unhooking)|
| File System Monitoring | Monitor system-wide file activity for suspicious patterns |
| Behavioral Analysis | Use baselines and machine learning to detect "normal" activity anomalies (Process Hollowing or privilege escalation)|



## 🚨 Troubleshooting

### Build Issues

**Problem**: `'cl' is not recognized`
**Solution**: Run `build.bat` from any command prompt - it auto-detects Visual Studio

**Problem**: `cannot open file 'built\keylogger.dll'`
**Solution**: DLL is loaded in a process. Restart target process first.

### Runtime Issues

**Problem**: No log file created
**Solution**: Check absolute path in code matches your system

**Problem**: DLL injection failed
**Solution**: 
- Run as Administrator
- Check target process architecture (x64)
- Verify process PID is correct

### Log File Issues

**Problem**: Only virtual key codes logged
**Solution**: Rebuild with updated character conversion code

**Problem**: Wrong characters logged
**Solution**: Check keyboard layout and Shift state detection

## 📚 Educational Value

This project demonstrates:
- **Windows API Programming**: Advanced API usage
- **Process Injection**: Modern injection techniques
- **Hook Programming**: System-wide hooks
- **Character Encoding**: Virtual key conversion
- **System Programming**: Low-level Windows internals

## 🔗 References

- [Microsoft Windows API Documentation](https://docs.microsoft.com/en-us/windows/win32/api/)
- [SetWindowsHookEx Function](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw)
- [Virtual Key Codes](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
- [DLL Injection Techniques](https://www.malwaretech.com/2015/01/dll-injection-techniques.html)

## 📄 License

This project is for educational purposes only. Use responsibly and in accordance with local laws and regulations.

---

**Created for educational and security research purposes. Always obtain proper authorization before use.**
