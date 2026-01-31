//==============================================================================
// Minamoto : Node Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxNode.h>

struct RuntimeAPI Node : public xxNode
{
public:
    enum
    {
        PARTICLE    = 0b00000001'00000000,
    };

public:
    bool        AttachChild(xxNodePtr const& child);
    bool        DetachChild(xxNodePtr const& child);

    void        Invalidate();
    void        Draw(xxDrawData const& data);

    static bool Traversal(xxNodePtr const& node, std::function<int(xxNodePtr const&)> const& callback);

    static void Initialize();
    static void Shutdown();
};

#if defined(xxWINDOWS)
#if defined(_LIBCPP_VERSION)
#pragma comment(linker, "/alternatename:__imp_?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=__imp_?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:__imp_?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=__imp_?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:__imp_?GetChild@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@_K@Z=__imp_?GetChild@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@_K@Z")
#pragma comment(linker, "/alternatename:__imp_?GetParent@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@XZ=__imp_?GetParent@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:__imp_?Traversal@xxNode@@SA_NAEBV?$shared_ptr@UNode@@@__1@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@UNode@@@__1@std@@@Z@34@@Z=__imp_?Traversal@xxNode@@SA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z@34@@Z")
#pragma comment(linker, "/alternatename:__imp_?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@UNode@@@__1@std@@XZEA=__imp_?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@VxxNode@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:__imp_?Create@xxNode@@SA?AV?$shared_ptr@UNode@@@__1@std@@XZ=__imp_?Create@xxNode@@SA?AV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:?GetChild@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@_K@Z=?GetChild@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@_K@Z")
#pragma comment(linker, "/alternatename:?GetParent@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@XZ=?GetParent@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?Traversal@xxNode@@SA_NAEBV?$shared_ptr@UNode@@@__1@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@UNode@@@__1@std@@@Z@34@@Z=?Traversal@xxNode@@SA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z@34@@Z")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@UNode@@@__1@std@@XZEA=?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@VxxNode@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxNode@@SA?AV?$shared_ptr@UNode@@@__1@std@@XZ=?Create@xxNode@@SA?AV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#else
#pragma comment(linker, "/alternatename:__imp_?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@std@@@Z=__imp_?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@std@@@Z")
#pragma comment(linker, "/alternatename:__imp_?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@std@@@Z=__imp_?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@std@@@Z")
#pragma comment(linker, "/alternatename:__imp_?GetChild@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@std@@_K@Z=__imp_?GetChild@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@std@@_K@Z")
#pragma comment(linker, "/alternatename:__imp_?GetParent@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@std@@XZ=__imp_?GetParent@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@std@@XZ")
#pragma comment(linker, "/alternatename:__imp_?Traversal@xxNode@@SA_NAEBV?$shared_ptr@UNode@@@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@UNode@@@std@@@Z@34@@Z=__imp_?Traversal@xxNode@@SA_NAEBV?$shared_ptr@VxxNode@@@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@VxxNode@@@std@@@Z@34@@Z")
#pragma comment(linker, "/alternatename:__imp_?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@UNode@@@std@@XZEA=__imp_?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@VxxNode@@@std@@XZEA")
#pragma comment(linker, "/alternatename:__imp_?Create@xxNode@@SA?AV?$shared_ptr@UNode@@@std@@XZ=__imp_?Create@xxNode@@SA?AV?$shared_ptr@VxxNode@@@std@@XZ")
#pragma comment(linker, "/alternatename:?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@std@@@Z=?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@std@@@Z")
#pragma comment(linker, "/alternatename:?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@std@@@Z=?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@std@@@Z")
#pragma comment(linker, "/alternatename:?GetChild@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@std@@_K@Z=?GetChild@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@std@@_K@Z")
#pragma comment(linker, "/alternatename:?GetParent@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@std@@XZ=?GetParent@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@std@@XZ")
#pragma comment(linker, "/alternatename:?Traversal@xxNode@@SA_NAEBV?$shared_ptr@UNode@@@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@UNode@@@std@@@Z@3@@Z=?Traversal@xxNode@@SA_NAEBV?$shared_ptr@VxxNode@@@std@@AEBV?$function@$$A6AHAEBV?$shared_ptr@VxxNode@@@std@@@Z@3@@Z")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@UNode@@@std@@XZEA=?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@VxxNode@@@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxNode@@SA?AV?$shared_ptr@UNode@@@std@@XZ=?Create@xxNode@@SA?AV?$shared_ptr@VxxNode@@@std@@XZ")
#endif
#endif
