@echo off
echo Compiling Graphics project...
cl /EHsc /I. *.cpp /Fe:Graphics.exe /link opengl32.lib glu32.lib freeglut.lib glew32.lib
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Run Graphics.exe to start the game
) else (
    echo Compilation failed with error code %ERRORLEVEL%
)
pause

