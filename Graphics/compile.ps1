Write-Host "Compiling Graphics project..." -ForegroundColor Green

# Try to find Visual Studio installation
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($vsPath) {
        Write-Host "Found Visual Studio at: $vsPath" -ForegroundColor Yellow
        $vcvars = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
        if (Test-Path $vcvars) {
            Write-Host "Setting up Visual Studio environment..." -ForegroundColor Yellow
            cmd /c "`"$vcvars`" && cl /EHsc /I. *.cpp /Fe:Graphics.exe /link opengl32.lib glu32.lib freeglut.lib glew32.lib"
            if ($LASTEXITCODE -eq 0) {
                Write-Host "Compilation successful! Run Graphics.exe to start the game." -ForegroundColor Green
            } else {
                Write-Host "Compilation failed with error code $LASTEXITCODE" -ForegroundColor Red
            }
        } else {
            Write-Host "vcvars64.bat not found" -ForegroundColor Red
        }
    } else {
        Write-Host "Visual Studio not found" -ForegroundColor Red
    }
} else {
    Write-Host "vswhere.exe not found, trying direct compilation..." -ForegroundColor Yellow
    try {
        cl /EHsc /I. *.cpp /Fe:Graphics.exe /link opengl32.lib glu32.lib freeglut.lib glew32.lib
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Compilation successful! Run Graphics.exe to start the game." -ForegroundColor Green
        } else {
            Write-Host "Compilation failed with error code $LASTEXITCODE" -ForegroundColor Red
        }
    } catch {
        Write-Host "Direct compilation failed: $_" -ForegroundColor Red
    }
}

Write-Host "Press any key to continue..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

