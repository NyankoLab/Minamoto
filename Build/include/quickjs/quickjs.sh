#!/bin/sh
# ==============================================================================
# Minamoto : quickjs Bash
# 
# Copyright (c) 2023-2026 TAiGA
# https://github.com/metarutaiga/minamoto
# ==============================================================================

if [ ! -f quickjs-ver.h ] || [ VERSION -nt quickjs-ver.h ]; then
  echo \#define CONFIG_VERSION \"$(cat VERSION)\" > quickjs-ver.h
  rm qjsc
  rm qjscalc.c
  rm repl.c
fi
if [ ! -f qjsc ]; then
  clang -I. -include ../../Build/include/quickjs-user.h -Ofast cutils.c dtoa.c libregexp.c libunicode.c qjsc.c quickjs.c quickjs-libc.c -o qjsc
fi
if [ ! -f repl.c ]; then
  ./qjsc -c -o repl.c -m repl.js
fi
