@echo off
REM A batch script to install the pywin32 library using pip.
REM This script is designed to be run directly in the Windows Command Prompt.

echo This script will install the pywin32 library.
echo The ctypes library is included with Python by default and does not need to be installed.
echo.

REM --- Step 1: Check if Python is installed ---
python --version >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: Python was not found in your system's PATH.
    echo Please install Python before running this script.
    echo You can download it from: https://www.python.org/downloads/
    echo.
    goto :end
)

echo Python was found. Continuing with pip check and installation.
echo.

REM --- Step 2: Check if pip is available and install pywin32 ---
pip --version >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: 'pip' was not found.
    echo This usually means Python was not installed with pip or the PATH is not set correctly.
    echo Please ensure Python and pip are installed and in your system's PATH.
    echo.
    goto :end
)

echo 'pip' was found.
echo.
echo Installing pywin32...
pip install pywin32

echo.
echo Installation complete.
echo.

:end
pause