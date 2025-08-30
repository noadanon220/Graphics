@echo off
echo Compiling Graphics project...

REM Try to find Visual Studio installation
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

if defined VS_PATH (
    echo Found Visual Studio at: %VS_PATH%
    call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
    echo Building with Visual Studio compiler...
    cl /EHsc /I. *.cpp /Fe:Graphics.exe /link opengl32.lib glu32.lib freeglut.lib glew32.lib
) else (
    echo Visual Studio not found. Trying to use system compiler...
    cl /EHsc /I. *.cpp /Fe:Graphics.exe /link opengl32.lib glu32.lib freeglut.lib glew32.lib
)

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful! Run Graphics.exe to start the game.
) else (
    echo Compilation failed with error code %ERRORLEVEL%
)

pause

