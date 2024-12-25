@echo off
REM Copyright (c) 2024, Ivan Reshetnikov - All rights reserved.

call lib_color.bat

set "ORIGINAL_DIR=%cd%"

set "MSVC__ENV_SETUP_SCRIPT_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build"
set "MSVC__USE_x32_BUILD_x32=vcvars32.bat"
set "MSVC__USE_x64_BUILD_x64=vcvars64.bat"

cd /d "%MSVC__ENV_SETUP_SCRIPT_PATH%"
call "%MSVC__USE_x64_BUILD_x64%"

cd /d "%ORIGINAL_DIR%"
echo [setup.bat] %COLOR_FG_GREEN%Finished environment setup!%COLOR_RESET%

goto :EOF