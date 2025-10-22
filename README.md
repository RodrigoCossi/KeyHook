# KeyHook - Advanced Keylogger with DLL Injection

A sophisticated Windows keylogger implementation using low-level keyboard hooks and DLL injection techniques. This project demonstrates advanced Windows API usage for educational and security research purposes.

## ⚠️ Legal Disclaimer

This software is intended for **educational purposes only**. Use responsibly and only on systems you own or have explicit permission to monitor. Unauthorized keylogging is illegal in many jurisdictions.

## 🚀 Features

- **System-wide Key Capture**: Low-level keyboard hook captures all keystrokes across the entire system
- **Process-Specific Console Monitoring**: Inline hook for targeted console application surveillance
- **DLL Injection**: Injects keylogger into target processes for stealth operation
- **Character Conversion**: Converts virtual key codes to readable characters
- **Special Key Support**: Handles Shift, Caps Lock, punctuation, and function keys
- **Real-time Logging**: Immediate keystroke logging to file
- **Dual Approach**: Global surveillance OR targeted console monitoring
- **Process Persistence**: Runs as long as the host process is active
- **x64 Architecture**: Compiled for 64-bit Windows systems

## 📁 Project Structure

```
KeyHook/
├── keyboardhook.cpp        # Main keylogger DLL with keyboard hook
├── injector.cpp            # DLL injector utility
├── inlinehook.cpp          # Alternative: ReadConsoleA inline hook
├── build.bat               # Automated build script
├── built/                  # Generated compiled files
│   ├── keylogger.dll       # Main keylogger DLL
│   ├── injector.exe        # DLL injection tool
│   └── inlinehook.dll      # ReadConsoleA hook DLL
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
   cl /LD /EHsc /Feinlinehook.dll inlinehook.cpp user32.lib kernel32.lib
   ```

## 🎯 Usage

### Step 1: Identify Host Process

Get the Process ID (PID) of a **host process** to run our keylogger code. The host process choice doesn't affect the scope of key capture (which is always system-wide), but affects stability and stealth:

**Recommended: `explorer.exe` (most stable)**
```powershell
Get-Process explorer | Select-Object Id,ProcessName
```

**Alternative host processes:**
```powershell
# Other stable processes you could use
Get-Process notepad | Select-Object Id,ProcessName     # If Notepad is open
Get-Process winlogon | Select-Object Id,ProcessName   # System process (requires admin)
```

**Output example:**
```
   Id ProcessName
   -- -----------
12345 explorer
```

**Important:** The PID is only used to choose **where our code runs**, not **what keys we capture**. The keylogger will capture system-wide keystrokes regardless of which process hosts it.

### Step 2: Inject Keylogger

Inject the keylogger DLL into the **host process** (this only determines where our code runs):

```powershell
.\built\injector.exe 12345 "C:\full\path\to\KeyHook\built\keylogger.dll"
```

**Expected output:**
```
DLL injected successfully.
```

**Note:** Once injected, the keylogger will capture keystrokes from **all applications system-wide**, not just from the host process.

### Alternative: Process-Specific Console Monitoring

For **targeted console monitoring** instead of system-wide capture, use the inline hook:

```powershell
# Step 1: Get target console process
Get-Process cmd | Select-Object Id,ProcessName        # Command Prompt
Get-Process powershell | Select-Object Id,ProcessName # PowerShell
Get-Process sqlcmd | Select-Object Id,ProcessName     # SQL Console

# Step 2: Inject inline hook for console-only monitoring
.\built\injector.exe <console_PID> "C:\full\path\to\KeyHook\built\inlinehook.dll"
```

**Console Hook Benefits:**
- ✅ **Precision**: Only console input (commands, passwords in CLI)
- ✅ **Stealth**: Lower detection risk than global hooks
- ✅ **Performance**: Minimal system impact
- ✅ **Targeted**: Focus on specific applications/workflows

## 🔄 Choosing the Right Approach

| Scenario | Use Global Hook (`keylogger.dll`) | Use Console Hook (`inlinehook.dll`) |
|----------|-----------------------------------|-------------------------------------|
| **Complete user surveillance** | ✅ **Best choice** | ❌ Too limited |
| **Monitor admin server commands** | ❌ Too much noise | ✅ **Perfect fit** |
| **Capture application passwords** | ✅ **Comprehensive** | ✅ **If console-based** |
| **Stealth operation** | ❌ High detection risk | ✅ **Lower signature** |
| **Database session monitoring** | ❌ Irrelevant data | ✅ **Precise targeting** |
| **General keylogging** | ✅ **Full coverage** | ❌ Misses GUI apps |

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

### 3. Inline Hook (`inlinehook.cpp`)

Alternative approach using **process-specific** function hooking:
- **ReadConsoleA Function Interception**: Patches console input API calls
- **Original Function Preservation**: Maintains normal application behavior
- **Console Input Monitoring**: Captures only command-line/console input
- **Targeted Surveillance**: Monitors specific processes, not system-wide

**Key Differences from Global Hook:**
- **Scope**: Process-specific vs. system-wide
- **Stealth**: Lower detection signature
- **Performance**: Minimal resource usage
- **Precision**: Console applications only

**Use Cases:**
- Monitor administrator command-line activities
- Capture database console sessions (sqlcmd, mysql)
- Track specific application console input
- Reduced-footprint surveillance

**Usage Example:**
```powershell
# Target a specific CMD or PowerShell window
Get-Process cmd | Select-Object Id,ProcessName
injector.exe <cmd_PID> "C:\path\to\built\inlinehook.dll"
```

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

### Windows Hook Architecture

Understanding how system-wide keylogging works through DLL injection:

```
[Keyboard Input] → [Windows Hook Manager] → [Our Callback in Host Process]
     ↑                        ↑                         ↓
All applications      System-wide router         Logs to file
```

**Key Points:**

1. **Hook Scope vs. Host Process**:
   - `WH_KEYBOARD_LL` creates **system-wide** hooks regardless of host process
   - Host process (explorer.exe) is just a **container** for our code
   - System-wide capability comes from **Windows hooking mechanism**, not the target process

2. **Why Any Process Works**:
   ```cpp
   // This line determines scope (system-wide), not the host process
   hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
   //                       ^^^^^^^^^^^^^^                                    ^
   //                       Hook type = system-wide              Thread ID = 0 (all threads)
   ```

3. **Host Process Selection Criteria**:
   | Process | Pros | Cons |
   |---------|------|------|
   | `explorer.exe` | Stable, persistent, legitimate | Well-known target |
   | `notepad.exe` | Simple, low footprint | User might close it |
   | `chrome.exe` | Common, high privileges | Resource intensive |
   | Custom process | Full control | Suspicious standalone process |

4. **Technical Flow**:
   ```
   1. Inject DLL into any process → Gets our code running
   2. DLL calls SetWindowsHookEx(WH_KEYBOARD_LL) → Windows installs SYSTEM-WIDE hook
   3. Windows routes ALL keyboard events → To our callback function
   4. Callback executes in host process → But captures keys from everywhere
   ```

**The magic is in `WH_KEYBOARD_LL` + thread ID `0`, not the host process location!**

## 🛡️ Security Considerations

### Detection Avoidance
- Runs within legitimate system processes
- No standalone process footprint
- Minimal API calls
- File logging can be customized/encrypted

### Potential Detection Methods
- Process module enumeration
- Hook chain inspection
- File system monitoring
- Behavioral analysis

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