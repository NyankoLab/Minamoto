@echo off
rem // ==============================================================================
rem // Minamoto : quickjs Batch
rem //
rem // Copyright (c) 2019-2024 TAiGA
rem // https://github.com/metarutaiga/minamoto
rem //==============================================================================

for /f %%s in ('dir /b /o:d VERSION quickjs-ver.h') do set newer=%%s
if not %newer%==quickjs-ver.h (
  for /f %%s in (VERSION) do echo #define CONFIG_VERSION "%%s" > quickjs-ver.h
  del qjsc.exe
  del qjscalc.c
  del repl.c
)
for /f %%s in ('dir /b /o:d quickjs.h quickjs.fixed.h') do set newer=%%s
if not %newer%==quickjs.fixed.h (
  powershell -Command "(Get-Content quickjs.h) -replace '\(JSValue\)v', 'v' | Set-Content quickjs.fixed.h"
)
for /f %%s in ('dir /b /o:d quickjs.c quickjs.fixed.c') do set newer=%%s
if not %newer%==quickjs.fixed.c (
  powershell -Command "(Get-Content quickjs.c) -replace 'quickjs.h', 'quickjs.fixed.h' | Set-Content quickjs.fixed.1"
  powershell -Command "(Get-Content quickjs.fixed.1) -replace ' \(JSValue\)', ' ' | Set-Content quickjs.fixed.2"
  powershell -Command "(Get-Content quickjs.fixed.2) -replace '1.0 / 0.0', 'INFINITY' | Set-Content quickjs.fixed.3"
  powershell -Command "(Get-Content quickjs.fixed.3) -replace ' \(JSValueConst\)', ' ' | Set-Content quickjs.fixed.c"
  del quickjs.fixed.1
  del quickjs.fixed.2
  del quickjs.fixed.3
)
if not exist qjsc.exe (
  cl -std:c11 -experimental:c11atomics -I. -I../../Build/include/quickjs-win32 -FI ../../Build/include/quickjs-user.h -Ox cutils.c libbf.c libregexp.c libunicode.c qjsc.c quickjs.fixed.c quickjs-libc.c -o qjsc.exe
)
if not exist qjscalc.c (
  qjsc.exe -c -o qjscalc.c -m qjscalc.js
)
if not exist repl.c (
  qjsc.exe -c -o repl.c -m repl.js
)
for /f %%s in ('dir /b /o:d quickjs.h quickjs.hpp') do set newer=%%s
if not %newer%==quickjs.hpp (
  powershell -Command "(Get-Content quickjs.h) -replace '\(JSValue\){ \(JSValueUnion\){', '{ {' | Set-Content quickjs.1"
  powershell -Command "(Get-Content quickjs.1) -replace '.u = {', '{' | Set-Content quickjs.hpp"
  del quickjs.1
)
