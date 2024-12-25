@echo off
:: Copyright (c) 2024, Ivan Reshetnikov - All rights reserved.

call lib_color.bat

set "FLAGS="
set "FLAGS=%FLAGS% /std:c++17"
set "FLAGS=%FLAGS% /W3"
set "FLAGS=%FLAGS% /O2"

set "SOURCE_FILES=./src/glad.c ./src/main.cpp ./src/logging.cpp ./src/shader_utils.cpp ./src/file_utils.cpp ./src/texture_utils.cpp"
set "OUT_FILENAME=./game/bin/main.exe"

set "LIB_TARGETS=shell32.lib SDL2.lib SDL2main.lib"

echo %COLOR_VIVID%[compile.bat] Cleaning up (shallow)%COLOR_RESET%
del %OUT_FILENAME%

cl %FLAGS% /I"./include" %SOURCE_FILES% /Fo"./obj/" /EHsc /link /LIBPATH:"./lib" %LIB_TARGETS% /out:%OUT_FILENAME% /subsystem:console

if %ERRORLEVEL% EQU 0 (
    echo.
    echo %COLOR_VIVID%[compile.bat] %COLOR_FG_GREEN%Compilation finished!%COLOR_RESET%
) else (
    echo.
    echo %COLOR_VIVID%[compile.bat] %COLOR_FG_RED%Compilation failed!%COLOR_RESET%
)