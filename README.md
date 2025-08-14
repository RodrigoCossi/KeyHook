# Key Hook - Exploiting DLL injections for Keylogging

⚠️ WORK IN PROGRESS ⚠️

## Intro about DLLs:

DLL injection is a technique used to run code within the address space of another process by forcing it to load a dynamic-link library (DLL) - a shared module used natively in Windows systems to control essential resources and functionalities. (https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-libraries).
This method allows the injected code to manipulate the target process's behavior, which can be used for both legitimate and malicious purposes.

The injected DLL can perform various actions, from altering the process's behavior to stealing sensitive information. It's a technique used in debugging, game modding, malware analysis, but also in data theft, bypassing protections, or functionality hijacking. 


> ⚠️ Disclaimer:
> This project is for educational and ethical purposes only — do not use its code or concepts in unauthorized environments. > Keylogging or injecting code into other processes is illegal if used maliciously.

</br>

## Project Overview:

This project has two main components:
<!-- <br>✅ A Custom DLL (`logger.dll` change to keyhook.dll?): Hooks keyboard events and logs them to a file (C:\keylog.txt). -->
<!-- <br>✅ An Injector (`keyhook.py` change to injector?): A script to inject the DLL into a running process (e.g., Notepad). -->
<br>✅ A Custom DLL (`logger.dll`): Hooks keyboard events and logs them to a file (C:\keylog.txt).
<br>✅ An Injector (`keyhook.py`): A script to inject the DLL into a running process (e.g., Notepad).

In this exercise, `keyhook.py` is a script performing a manual DLL injection into a Notepad process, to perform keystroke logging via `logger.dll`.

</br>
(Add images here)
</br>
</br>

### Setup:

- this project uses Python `ctypes` and `pywin32` libraries to simulate C/C++ interaction with operating system-level functions. Run `install.bat` to auto-install all the necessary python dependencies if they are missing. Then, run `compile_logger.bat` from `x64 Native Tools Command Prompt for VS` to generate a 64-bit `logger.dll` binary. (Notepad is a 64-bit app on Windows 11. You may want to check Nodepad's bitness on your machine first)  


- Open a shell and launch a targer process. For example, `notepad.exe`. Keep it running.


- Obtain the Process ID (PID).
        - On the same shell, run `tasklist | findstr notepad`. It will look like:
        notepad.exe              10892 Console ...
        `10892` is the PID.


</br>

### Execution:

- Run the injector script with the correct PID. Example: `python inject.py 10892`

- The DLL’s DllMain() runs inside Notepad.

        - It starts a thread that:

                Sets up a low-level keyboard hook

                Logs keystrokes to C:\keylog.txt

- Try typing anywhere — e.g., Notepad, browser, terminal. Check: `type C:\keylog.txt`

- You should see key logs, like:
        HELLO[13]WORLD[32][8]!
        (Where [13] = Enter, [32] = Space, etc.)



</br>

### Cleanup:

- Closing the target process unloads the DLL. Delete C:\keylog.txt and test.dll if needed.

---

## 🧠 What Is the Injector (`keyhook.py`) Actually Doing to Notepad?

The injection process causes **Notepad** (or any target process) to:

1. **Load your custom DLL** into its memory space
   Using the `LoadLibraryA` function, the injector forces the target process to load your DLL.

2. **Execute the DLL’s `DllMain()` function**
   This function is called automatically by the system, typically with the reason `DLL_PROCESS_ATTACH`, allowing your code to run as part of the target process.

---

## ⚙️ How DLL Injection Works

1. **Find the Target Process**
   <br> Use Windows APIs like `OpenProcess()` to get a handle to the process you want to inject into.

2. **Allocate Memory**
   <br> Allocate memory inside the target process using `VirtualAllocEx()`.

3. **Write DLL Path**
   <br> Write the path to the DLL into that memory using `WriteProcessMemory()`.

4. **Create Remote Thread**
   <br> Use `CreateRemoteThread()` to run `LoadLibrary()` inside the target process, which loads the DLL you planted.

### 🔐 Result

The injected DLL now runs **as if it’s part of the target application**, sharing its memory and privileges.

---

🧭 2. Visual Diagram: DLL Injection Flow

        [Injector Process - KeyHook.py]
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
        ┌─────────▼────────────────────────┐
        │ HookThread():                    │
        │ - SetWindowsHookEx(WH_KEYBOARD)  │ ← Installs low-level keyboard hook
        │ - Loop: GetMessage/Dispatch      │ ← Keeps hook alive (event loop)
        └──────────────────────────────────┘


---

## 💥 What Could the DLL Do to Notepad?

Once injected into Notepad’s process space, your DLL has **full access** to its memory, windows, UI, keyboard events, and more. Here’s what it could potentially do:

---

### 🧪 1. Monitor or Manipulate Text Input

* Hook into Notepad's **text buffer** to log user input.
* **Automatically modify** text typed into the editor in real time.

---

### 🎯 2. Hook API Calls

* Intercept system calls like `WriteFile`, `ReadFile`, `SendMessage`, etc.
* Modify or redirect Notepad’s behavior at **runtime**.

---

### 🔍 3. Read or Write Process Memory

* Scan Notepad’s memory for sensitive content (e.g., typed text, clipboard).
* **Exfiltrate** in-memory data or inject custom content.

---

### 🧨 4. Inject Code into Other Threads

* Create or hijack threads within Notepad.
* Use Notepad as a **stealth container** to run hidden operations.

---

### 🔁 5. Use Notepad as a Launchpad

* Launch additional processes or payloads from within Notepad.
* Communicate with a **C2 server** using Notepad’s identity to evade detection.

---



<!-- HOT: 🕷️ Example: Stealing Browser Cookies 
Inject into chrome.exe, hook functions that handle cookies (like InternetGetCookie), and dump them to a file or C2 server.
-->
 

--------------------------------------------------




<!-- 
🛡️ Blue Team Tips:
Monitor for calls to VirtualAllocEx, WriteProcessMemory, CreateRemoteThread.

Look for DLLs being loaded from unusual paths.

Use Windows Defender's ASR rules, AppLocker, or EDR tools.


🔒 For Blue Teamers & Analysts:
Monitor for:

Unusual API usage: VirtualAllocEx, CreateRemoteThread, WriteProcessMemory

DLLs loaded from non-standard paths

Processes launching with no UI

Sysmon logs (event ID 10 – process access, ID 7 – image loaded)




🛡️ Defense Against DLL Injection:

| Technique               | Defense                                   |
| ----------------------- | ----------------------------------------- |
| Code injection          | Use endpoint protection (EDR/AV)          |
| Suspicious DLL behavior | Monitor with behavior-based detection     |
| Unusual memory changes  | Detect with security tools like Sysmon    |
| Unauthorized loading    | Use **DLL whitelisting** or **AppLocker** |
| ASLR & DEP              | Enable to prevent exploitation            |



🛡️ Detection and Defense (Blue Team Awareness):
Use EDR or Sysmon to detect SetWindowsHookEx used in unusual processes

Monitor for writes to unusual locations like C:\keylog.txt

Check for DLLs injected into UI-heavy apps (e.g., Notepad, Explorer)


--------------------------------------------READ LATER------------------------------

🧩 Core Logic Breakdown
PID Stub (pid = 1234) This is a placeholder. You're expected to replace 1234 with the actual Process ID (PID) of the target process you want to inject into (e.g., Notepad, calc.exe). ⚠️ The script itself does not retrieve the PID — it assumes you already have it from somewhere else.

Open the Target Process Uses OpenProcess with specific flags to gain permissions like thread creation and memory operations.

Allocate Memory in Target Process Calls VirtualAllocEx to reserve space inside the target's address space to store the DLL path.

Write DLL Path WriteProcessMemory copies the path to the previously allocated buffer.

Get Address of LoadLibraryA It fetches the memory address of LoadLibraryA from kernel32.dll. This function will load your DLL when executed in the context of the remote process.

Create Remote Thread Calls CreateRemoteThread with LoadLibraryA and the path argument, triggering the DLL to be loaded.

Print Outcome Simple success/failure message based on whether the thread was created.


🔍 What It Doesn't Do
It doesn't:

Verify if the PID is valid.

Scan processes to find one by name.

Confirm injection success beyond thread creation.

Retrieve any real-time PID using something like psutil, win32process.EnumProcesses, or WMI.

If you're up for extending it, I can help you write a snippet that scans for a process name like "notepad.exe" and grabs its PID automatically. Want to go there next?

can you use a dll injection for key logging? is that a common key logging mechanism? What technicques are common to capture keystrokes among key loggers?
Absolutely — DLL injection can be used for keylogging, but it’s just one of several techniques in the wild. Let’s break it down:

💉 DLL Injection for Keylogging
How it works: A DLL containing a keyboard hook (usually via SetWindowsHookEx) is injected into a target process. Once inside, it can intercept keystrokes system-wide or within specific threads.

Use case: Often used in educational demos, malware, or monitoring tools. It’s effective but not stealthy by default — hooking APIs can be flagged by security software.

Example: Projects like this one on GitHub demonstrate DLL injection combined with global hooks to log keystrokes and save them to encrypted files.

🧠 Common Keylogging Techniques
Here’s a rundown of popular methods used by keyloggers:

Technique	Description	Stealth Level
SetWindowsHookEx	Hooks keyboard events via WinAPI; often used in DLL-based keyloggers	Medium
Polling APIs	Uses GetAsyncKeyState or GetKeyState to check key status repeatedly	Low
Kernel-mode drivers	Installs a driver to intercept keystrokes at the OS level	High
Filter drivers	Hooks into input stack at a lower level than user-mode hooks	High
Custom input DLLs	Injects into processes and replaces legitimate input handlers	Medium–High
Registry manipulation	Alters system settings to load malicious input handlers	Medium
Hardware keyloggers	Physical devices between keyboard and system	Very High
🧪 Reflective & Advanced Injection Variants
Reflective DLL Injection: Loads a DLL from memory without touching disk — harder to detect.

DLL Hollowing / Module Stomping: Overwrites a legit DLL’s entry point with malicious code.

Process Doppelgänging: Uses NTFS transactions to mask injected code.

If you're exploring this from a security research or educational angle, I can help you build a minimal hook DLL or simulate different capture methods. Want to try crafting a stealthier version or compare polling vs hooking performance?

Yesterday
"This script is performing manual DLL injection into a remote process on Windows". What is the "remote" process?
Excellent catch — the term remote in “remote process” might sound like it’s talking about another computer, but it’s actually much simpler (and sneakier 😉).

🧠 What Is a "Remote Process"?
In this context, “remote” just means a process that your injector is targeting — but isn’t itself.

The injector (your script or tool) is one process.

The target process (like Notepad, Calculator, Chrome, etc.) is a separate, already-running program.

When we say "remote process," we’re talking about that target — from the injector’s perspective, it's external, or remote, even though it's on the same machine.

📦 Why It Matters
Normally, a program can only change its own memory and threads. But with Windows APIs like:

OpenProcess

VirtualAllocEx

WriteProcessMemory

CreateRemoteThread

...your injector gains access to another process’s memory space — allowing you to manipulate that "remote" entity. That’s where the magic (or mischief) happens.

Would you like to go deeper into how processes are sandboxed and how Windows enforces boundaries — or how these APIs bypass them when permission is granted? I can break down the security model next 🛡️🧬.

manual DLL injection vs DLL injection
Great question, Rodrigo — the phrase “manual DLL injection” tends to throw people off because it sounds like a subset, but it actually describes a distinct method with deeper control. Let’s break it down surgically 🧵:

💉 DLL Injection (Standard)
This is the umbrella term for any technique that forces a DLL into the memory space of another process.

✅ Common methods: CreateRemoteThread, SetWindowsHookEx, AppInit_DLLs, etc.

🧠 Windows loader is still involved — you point to a DLL path, and the OS loads it via standard mechanisms (LoadLibrary).

This is simpler, widely used, and often detectable by basic security tools.

🔬 Manual DLL Injection
This is the deep end of the pool.

🛠 You manually map the DLL into the target process memory:

Parse headers (IMAGE_DOS_HEADER, IMAGE_NT_HEADERS)

Allocate memory (VirtualAllocEx)

Copy sections manually (WriteProcessMemory)

Fix imports and relocations

Call entry point manually (CreateRemoteThread or hijack a thread)

Unlike standard injection, you bypass LoadLibrary entirely — the OS doesn't know a DLL was loaded the usual way.

🆚 Comparison
Feature	Standard DLL Injection	Manual DLL Injection
OS involvement	Uses Windows loader	No OS loader involvement
Stealth level	Lower	Higher
Complexity	Easier	Advanced
AV/EDR detection	More likely	Less likely (if done right)
Use cases	Scripted tools, trainers	Malware, red teaming, custom loaders

-->
