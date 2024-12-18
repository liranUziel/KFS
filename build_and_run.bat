:: filepath: /build_and_run.bat

@echo off
setlocal

:: Define variables
set SRC=.\Src\Main.c .\Src\BlockDevice.c .\Src\KFS.c .\Src\Terminal.c 
set OUT_DIR=.\Build
set OUT_EXE=%OUT_DIR%\Main.exe
set BLOCK_DEVICE=.\Build\gBlk.bin

:: Create the output directory if it doesn't exist
if not exist %OUT_DIR% (
    mkdir %OUT_DIR%
)

:: Compile the code
gcc %SRC% -o %OUT_EXE%

:: Check if the compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)

:: Run the executable with the block device file
%OUT_EXE% %BLOCK_DEVICE%

endlocal