@echo off
REM Define the output DLL name
set OUTPUT_DLL_NAME=logger.dll

REM Define the source file
set SOURCE_FILE=logger.cpp

REM Define the libraries to link against
REM user32.lib is needed for most Windows API functions like SetWindowsHookEx, GetMessage, MessageBox etc.
set LIBS=user32.lib

REM Compile the C++ source into an object file and then link it into a DLL
REM /LD: Creates a DLL
REM /EHsc: Enables C++ exception handling semantics (addresses warning C4530)
REM /Fe%OUTPUT_DLL_NAME%: Specifies the output file name
cl /LD /EHsc /Fe%OUTPUT_DLL_NAME% %SOURCE_FILE% %LIBS%

IF %ERRORLEVEL% NEQ 0 (
    echo.
    echo --- Compilation Failed ---
) ELSE (
    echo.
    echo --- Compilation Successful! ---
)

echo.
pause