@echo off
echo Compiling Graphics project...
cl /EHsc /I. *.cpp /Fe:Graphics.exe /link opengl32.lib glu32.lib freeglut.lib glew32.lib
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
) else (
    echo Compilation failed!
)
pause

