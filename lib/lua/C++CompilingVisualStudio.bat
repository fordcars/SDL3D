@echo off
REM Originally by Dennis D. Spreen, http://blog.spreendigital.de/2015/01/16/how-to-compile-lua-5-3-0-for-windows/
REM Modified by Carl Hewett

REM Modified to compile as C++ code
REM Compiled files are left in their directory

REM Save the original working directory to go back to it later
set originalDir=%cd%
set luaSourceDirectory=lua-5.3.2/src

echo Compiling Lua's source at '%luaSourceDirectory%' using VisualStudio's C^+^+ compiler

cd "%luaSourceDirectory%"

REM Compile
cl /TP /EHsc /MD /O2 /c /DLUA_BUILD_AS_DLL *.c
ren lua.obj lua.o
ren luac.obj luac.o

REM Link
link /DLL /IMPLIB:lua.lib /OUT:lua.dll *.obj 
link /OUT:lua.exe lua.o lua.lib 
lib /OUT:lua-static.lib *.obj
link /OUT:luac.exe luac.o lua-static.lib

REM Move the files back to the original dir
move lua.lib "%originalDir%/lua.lib"
move lua.dll "%originalDir%/lua.dll"
move lua.exe "%originalDir%/lua.exe"
move lua-static.lib "%originalDir%/lua-static.lib"
move luac.exe "%originalDir%/luac.exe"

REM Cd back to the original dir to be nice
cd "%originalDir%"