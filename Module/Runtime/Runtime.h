//==============================================================================
// Minamoto : Runtime Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <xxGraphic/xxGraphic.h>

#ifndef RuntimeAPI
#   if defined(_MSC_VER) && defined(RUNTIME_BUILD_LIBRARY)
#       define RuntimeAPI           __declspec(dllexport)
#   elif defined(_MSC_VER)
#       define RuntimeAPI           __declspec(dllimport)
#   else
#       define RuntimeAPI           __attribute__((visibility("default")))
#   endif
#endif

#if defined(_MSC_VER)
#   pragma warning(disable:4200)
#   pragma warning(disable:4251)
#endif

struct RuntimeAPI Runtime
{
    static void Initialize();
    static void Shutdown(bool suspend = false);
    static char const* Version();
};
