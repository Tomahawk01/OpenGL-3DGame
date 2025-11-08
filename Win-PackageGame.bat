@echo off
setlocal enabledelayedexpansion

echo Packaging Game...

set CONFIG_FILE=GameLib\src\Game\config.h
set GAME_EXE=bin\Release-windows-x86_64\Game\Game.exe
set RESOURCES_PATH=Game\assets\resources
set LEVELS_PATH=Templates
set TEMP_DIR=temp_package

if not exist "%CONFIG_FILE%" (
    echo Error: Config file not found at %CONFIG_FILE%
    pause
    exit /b 1
)

echo Reading version from %CONFIG_FILE%

for /f "tokens=*" %%a in ('findstr /c:"MAJOR = " "%CONFIG_FILE%"') do (
    for /f "tokens=6" %%b in ("%%a") do set MAJOR=%%b
)

for /f "tokens=*" %%a in ('findstr /c:"MINOR = " "%CONFIG_FILE%"') do (
    for /f "tokens=6" %%b in ("%%a") do set MINOR=%%b
)

for /f "tokens=*" %%a in ('findstr /c:"PATCH = " "%CONFIG_FILE%"') do (
    for /f "tokens=6" %%b in ("%%a") do set PATCH=%%b
)

set MAJOR=%MAJOR:;=%
set MINOR=%MINOR:;=%
set PATCH=%PATCH:;=%

set VERSION=v%MAJOR%.%MINOR%.%PATCH%
set ZIP_NAME=Game-%VERSION%.zip

echo Found version: %VERSION%
echo Creating package: %ZIP_NAME%

if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%" >nul 2>&1

echo Creating temporary package directory
mkdir "%TEMP_DIR%" >nul 2>&1

echo Copying files
copy "%GAME_EXE%" "%TEMP_DIR%\Game.exe" >nul

echo Copying resources file
copy "%RESOURCES_PATH%" "%TEMP_DIR%\resources" >nul
set RESOURCES_RESULT=%errorlevel%

echo Copying config file
copy "Game\.config" "%TEMP_DIR%\.config" >nul
set CONFIG_COPY_RESULT=%errorlevel%

echo Copying levels
robocopy "%LEVELS_PATH%" "%TEMP_DIR%\levels" /e /np /nfl /ndl /njh /njs
set LEVELS_RESULT=%errorlevel%

if %RESOURCES_RESULT% neq 0 (
    echo Error: Resources copy failed with exit code %RESOURCES_RESULT%
    rmdir /s /q "%TEMP_DIR%" >nul 2>&1
    pause
    exit /b 1
)

if %LEVELS_RESULT% geq 8 (
    echo Error: Levels copy failed with exit code %LEVELS_RESULT%
    rmdir /s /q "%TEMP_DIR%" >nul 2>&1
    pause
    exit /b 1
)

if exist "%ZIP_NAME%" (
    echo Removing existing %ZIP_NAME%
    del "%ZIP_NAME%" >nul
)

echo Creating zip file
powershell -command "Compress-Archive -Path '%TEMP_DIR%\*' -DestinationPath '%ZIP_NAME%' -CompressionLevel Optimal" >nul

echo Cleaning up...
rmdir /s /q "%TEMP_DIR%" >nul 2>&1

echo.
echo Packing complete! Created %ZIP_NAME%
pause