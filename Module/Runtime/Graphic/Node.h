//==============================================================================
// Minamoto : Node Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxNode.h>

struct RuntimeAPI Node : public xxNode
{
public:
    bool        AttachChild(xxNodePtr const& child);
    bool        DetachChild(xxNodePtr const& child);

    static bool Traversal(xxNodePtr const& node, std::function<bool(xxNodePtr const&)> callback);

    static void Initialize();
    static void Shutdown();
};

#if defined(xxWINDOWS)
#pragma comment(linker, "/alternatename:__imp_?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=__imp_?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:__imp_?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=__imp_?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:__imp_?GetChild@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@_K@Z=__imp_?GetChild@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@_K@Z")
#pragma comment(linker, "/alternatename:__imp_?GetParent@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@XZ=__imp_?GetParent@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:__imp_?Traversal@xxNode@@SA_NAEBV?$shared_ptr@UNode@@@__1@std@@V?$function@$$A6A_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z@34@@Z=__imp_?Traversal@xxNode@@SA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@V?$function@$$A6A_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z@34@@Z")
#pragma comment(linker, "/alternatename:__imp_?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@UNode@@@__1@std@@XZEA=__imp_?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@VxxNode@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:__imp_?Create@xxNode@@SA?AV?$shared_ptr@UNode@@@__1@std@@XZ=__imp_?Create@xxNode@@SA?AV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=?AttachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z=?DetachChild@xxNode@@QEAA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z")
#pragma comment(linker, "/alternatename:?GetChild@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@_K@Z=?GetChild@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@_K@Z")
#pragma comment(linker, "/alternatename:?GetParent@xxNode@@QEBAAEBV?$shared_ptr@UNode@@@__1@std@@XZ=?GetParent@xxNode@@QEBAAEBV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?Traversal@xxNode@@SA_NAEBV?$shared_ptr@UNode@@@__1@std@@V?$function@$$A6A_NAEBV?$shared_ptr@UNode@@@__1@std@@@Z@34@@Z=?Traversal@xxNode@@SA_NAEBV?$shared_ptr@VxxNode@@@__1@std@@V?$function@$$A6A_NAEBV?$shared_ptr@VxxNode@@@__1@std@@@Z@34@@Z")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@UNode@@@__1@std@@XZEA=?BinaryCreate@xxNode@@2P6A?AV?$shared_ptr@VxxNode@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxNode@@SA?AV?$shared_ptr@UNode@@@__1@std@@XZ=?Create@xxNode@@SA?AV?$shared_ptr@VxxNode@@@__1@std@@XZ")
#endif
