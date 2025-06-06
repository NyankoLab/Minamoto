//==============================================================================
// Minamoto : QuickJS Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include <xxGraphicPlus/xxFile.h>
#include "QuickJS.h"

extern "C"
{
#include <quickjs/quickjs-ver.h>
#include "../../Build/include/quickjs-user.h"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wc99-designator"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <quickjs/quickjs.h>
#elif defined(_MSC_VER)
#include <quickjs/quickjs.hpp>
#endif
}

JSRuntime* QuickJS::rt;
JSContext* QuickJS::ctx;
void (*QuickJS::dump_error)(struct JSContext*);
char const QuickJS::Version[] = "QuickJS " CONFIG_VERSION;
//==============================================================================
void QuickJS::Initialize()
{
    if (rt == nullptr)
    {
        static JSMallocFunctions const funcs =
        {
            [](JSMallocState* s, size_t size) -> void*              { return xxAlloc(char, size); },
            [](JSMallocState* s, void* ptr)                         { xxFree(ptr); },
            [](JSMallocState* s, void* ptr, size_t size) -> void*   { return xxRealloc(ptr, char, size); },
            [](const void* ptr) -> size_t                           { return xxAllocSize(ptr); },
        };

        rt = JS_NewRuntime2(&funcs, nullptr);
        ctx = JS_NewContext(rt);

        RuntimeLibrary();
    }
}
//------------------------------------------------------------------------------
void QuickJS::Shutdown()
{
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    rt = nullptr;
    ctx = nullptr;
}
//------------------------------------------------------------------------------
void QuickJS::RuntimeLibrary()
{
    extern JSModuleDef* js_init_module_engine(JSContext*);
    js_init_module_engine(ctx);
}
//------------------------------------------------------------------------------
void QuickJS::Eval(char const* buf, size_t len)
{
    JSValue val = JS_Eval(ctx, buf, len, "<QuickJS>", JS_EVAL_TYPE_MODULE);
    if (JS_IsException(val))
    {
        if (dump_error)
            dump_error(ctx);
        JS_ResetUncatchableError(ctx);
    }
    JS_FreeValue(ctx, val);
}
//------------------------------------------------------------------------------
void QuickJS::Update()
{
    for (int i = 0; i < 1000; ++i)
    {
        JSContext* ctx1;
        int err = JS_ExecutePendingJob(rt, &ctx1);
        if (err == 0)
            break;
        if (err < 0)
        {
            if (dump_error)
                dump_error(ctx1);
        }
    }
}
//==============================================================================
//  Engine I/O
//==============================================================================
static JSValue js_engine_log(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv)
{
    std::string temp;
    for (int i = 0; i < argc; i++)
    {
        if (i != 0)
            temp.push_back(' ');
        size_t len = 0;
        char const* str = JS_ToCStringLen(ctx, &len, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        temp.append(str, len);
        JS_FreeCString(ctx, str);
    }
    xxLog("QuickJS", "%s", temp.c_str());
    return JS_UNDEFINED;
}
//------------------------------------------------------------------------------
static JSClassID js_engine_file_class_id;
static JSClassDef js_engine_file_class =
{
    .class_name = "FILE",
    .finalizer = [](JSRuntime*, JSValue val)
    {
        xxFile* file = (xxFile*)JS_GetOpaque(val, js_engine_file_class_id);
        delete file;
    },
};
//------------------------------------------------------------------------------
static JSValue js_engine_file(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv, int magic)
{
    char const* filename = JS_ToCString(ctx, argv[0]);
    if (filename == nullptr)
        return JS_EXCEPTION;
    xxFile* file = (magic == 0) ? xxFile::Load(filename) : xxFile::Save(filename);
    JS_FreeCString(ctx, filename);
    if (file == nullptr)
        return JS_EXCEPTION;
    JSValue obj = JS_NewObjectClass(ctx, js_engine_file_class_id);
    if (JS_IsException(obj))
        return obj;
    JS_SetOpaque(obj, file);
    return obj;
}
//------------------------------------------------------------------------------
static JSValue js_engine_file_func(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv, int magic)
{
    xxFile* file = (xxFile*)JS_GetOpaque(this_val, js_engine_file_class_id);
    if (file == nullptr)
        return JS_EXCEPTION;
    switch (magic)
    {
    case 0:
    {
        int64_t len = 0;
        if (JS_ToInt64(ctx, &len, argv[0]))
            return JS_EXCEPTION;
        uint8_t* buf = xxAlloc(uint8_t, len);
        if (buf == nullptr)
            return JS_EXCEPTION;
        len = file->Read(buf, len);
        return JS_NewArrayBuffer(ctx, buf, len, [](JSRuntime*, void*, void* ptr) { xxFree(ptr); }, nullptr, 0);
    }
    case 1:
    {
        size_t len = 0;
        uint8_t* buf = JS_GetArrayBuffer(ctx, &len, argv[0]);
        if (buf == nullptr)
            return JS_EXCEPTION;
        len = file->Write(buf, len);
        return JS_NewInt64(ctx, len);
    }
    case 2:
        return JS_NewInt64(ctx, file->Position());
    case 3:
        return JS_NewInt64(ctx, file->Size());
    default:
        break;
    }
    return JS_EXCEPTION;
}
//------------------------------------------------------------------------------
JSModuleDef* js_init_module_engine(JSContext* ctx)
{
    static JSCFunctionListEntry const js_engine_funcs[] =
    {
        JS_PROP_STRING_DEF("Compiler", Runtime::Compiler, 0 ),
        JS_PROP_STRING_DEF("Target", Runtime::Target, 0 ),
        JS_PROP_STRING_DEF("Version", Runtime::Version, 0 ),
        JS_CFUNC_DEF("Log", 0, js_engine_log),
        JS_CFUNC_MAGIC_DEF("FileLoad", 1, js_engine_file, 0),
        JS_CFUNC_MAGIC_DEF("FileSave", 1, js_engine_file, 1),
    };

    auto js_engine_init = [](JSContext* ctx, JSModuleDef* m)
    {
        JSRuntime* rt = JS_GetRuntime(ctx);

        if (true || js_engine_file_class_id)
        {
            static JSCFunctionListEntry const js_engine_file_proto_funcs[] =
            {
                JS_CFUNC_MAGIC_DEF("Read", 1, js_engine_file_func, 0),
                JS_CFUNC_MAGIC_DEF("Write", 1, js_engine_file_func, 1),
                JS_CFUNC_MAGIC_DEF("Position", 0, js_engine_file_func, 2),
                JS_CFUNC_MAGIC_DEF("Size", 0, js_engine_file_func, 3),
            };

            JS_NewClassID(&js_engine_file_class_id);
            JS_NewClass(rt, js_engine_file_class_id, &js_engine_file_class);
            JSValue proto = JS_NewObject(ctx);
            JS_SetPropertyFunctionList(ctx, proto, js_engine_file_proto_funcs, xxCountOf(js_engine_file_proto_funcs));
            JS_SetClassProto(ctx, js_engine_file_class_id, proto);
        }

        JS_SetModuleExportList(ctx, m, js_engine_funcs, xxCountOf(js_engine_funcs));
        return 0;
    };

    JSModuleDef* m = JS_NewCModule(ctx, "Engine", js_engine_init);
    if (m)
    {
        JS_AddModuleExportList(ctx, m, js_engine_funcs, xxCountOf(js_engine_funcs));
    }
    return m;
}
//==============================================================================
//  Win32 Exports
//==============================================================================
#if defined(_WIN32)
#pragma comment(lib, "quickjs")
#if defined(_M_IX86)
#pragma comment(linker, "/export:___JS_FreeValue")
#pragma comment(linker, "/export:___JS_FreeValueRT")
#pragma comment(linker, "/export:_dbuf_free")
#pragma comment(linker, "/export:_dbuf_init2")
#pragma comment(linker, "/export:_dbuf_printf")
#pragma comment(linker, "/export:_dbuf_put")
#pragma comment(linker, "/export:_dbuf_putc")
#pragma comment(linker, "/export:_dbuf_putstr")
#pragma comment(linker, "/export:_has_suffix")
#pragma comment(linker, "/export:_JS_AddModuleExport")
#pragma comment(linker, "/export:_JS_AddModuleExportList")
#pragma comment(linker, "/export:_JS_AtomToCString")
#pragma comment(linker, "/export:_JS_Call")
#pragma comment(linker, "/export:_JS_DefinePropertyValue")
#pragma comment(linker, "/export:_JS_DefinePropertyValueStr")
#pragma comment(linker, "/export:_JS_DefinePropertyValueUint32")
#pragma comment(linker, "/export:_JS_Eval")
#pragma comment(linker, "/export:_JS_EvalFunction")
#pragma comment(linker, "/export:_JS_ExecutePendingJob")
#pragma comment(linker, "/export:_js_free_rt")
#pragma comment(linker, "/export:_js_free")
#pragma comment(linker, "/export:_JS_FreeAtom")
#pragma comment(linker, "/export:_JS_FreeContext")
#pragma comment(linker, "/export:_JS_FreeCString")
#pragma comment(linker, "/export:_JS_FreeRuntime")
#pragma comment(linker, "/export:_JS_GetArrayBuffer")
#pragma comment(linker, "/export:_JS_GetClassProto")
#pragma comment(linker, "/export:_JS_GetException")
#pragma comment(linker, "/export:_JS_GetGlobalObject")
#pragma comment(linker, "/export:_JS_GetImportMeta")
#pragma comment(linker, "/export:_JS_GetModuleName")
#pragma comment(linker, "/export:_JS_GetOpaque")
#pragma comment(linker, "/export:_JS_GetOpaque2")
#pragma comment(linker, "/export:_JS_GetPropertyStr")
#pragma comment(linker, "/export:_JS_GetRuntime")
#pragma comment(linker, "/export:_JS_GetRuntimeOpaque")
#pragma comment(linker, "/export:_JS_GetScriptOrModuleName")
#pragma comment(linker, "/export:_JS_IsError")
#pragma comment(linker, "/export:_JS_IsFunction")
#pragma comment(linker, "/export:_JS_LoadModule")
#pragma comment(linker, "/export:_js_malloc")
#pragma comment(linker, "/export:_js_mallocz")
#pragma comment(linker, "/export:_JS_NewArray")
#pragma comment(linker, "/export:_JS_NewArrayBufferCopy")
#pragma comment(linker, "/export:_JS_NewAtomLen")
#pragma comment(linker, "/export:_JS_NewBigInt64")
#pragma comment(linker, "/export:_JS_NewCFunction2")
#pragma comment(linker, "/export:_JS_NewClass")
#pragma comment(linker, "/export:_JS_NewClassID")
#pragma comment(linker, "/export:_JS_NewCModule")
#pragma comment(linker, "/export:_JS_NewContext")
#pragma comment(linker, "/export:_JS_NewObject")
#pragma comment(linker, "/export:_JS_NewObjectClass")
#pragma comment(linker, "/export:_JS_NewObjectProtoClass")
#pragma comment(linker, "/export:_JS_NewPromiseCapability")
#pragma comment(linker, "/export:_JS_NewRuntime")
#pragma comment(linker, "/export:_JS_NewString")
#pragma comment(linker, "/export:_JS_NewStringLen")
#pragma comment(linker, "/export:_JS_ParseJSON2")
#pragma comment(linker, "/export:_JS_PromiseResult")
#pragma comment(linker, "/export:_JS_PromiseState")
#pragma comment(linker, "/export:_JS_ReadObject")
#pragma comment(linker, "/export:_js_realloc_rt")
#pragma comment(linker, "/export:_JS_ResetUncatchableError")
#pragma comment(linker, "/export:_JS_ResolveModule")
#pragma comment(linker, "/export:_JS_RunGC")
#pragma comment(linker, "/export:_JS_SetCanBlock")
#pragma comment(linker, "/export:_JS_SetClassProto")
#pragma comment(linker, "/export:_JS_SetConstructor")
#pragma comment(linker, "/export:_JS_SetInterruptHandler")
#pragma comment(linker, "/export:_JS_SetModuleExport")
#pragma comment(linker, "/export:_JS_SetModuleExportList")
#pragma comment(linker, "/export:_JS_SetModuleLoaderFunc")
#pragma comment(linker, "/export:_JS_SetOpaque")
#pragma comment(linker, "/export:_JS_SetPropertyFunctionList")
#pragma comment(linker, "/export:_JS_SetPropertyStr")
#pragma comment(linker, "/export:_JS_SetPropertyUint32")
#pragma comment(linker, "/export:_JS_SetRuntimeOpaque")
#pragma comment(linker, "/export:_JS_SetSharedArrayBufferFunctions")
#pragma comment(linker, "/export:_JS_Throw")
#pragma comment(linker, "/export:_JS_ThrowOutOfMemory")
#pragma comment(linker, "/export:_JS_ThrowRangeError")
#pragma comment(linker, "/export:_JS_ThrowReferenceError")
#pragma comment(linker, "/export:_JS_ThrowTypeError")
#pragma comment(linker, "/export:_JS_ToBool")
#pragma comment(linker, "/export:_JS_ToCStringLen2")
#pragma comment(linker, "/export:_JS_ToFloat64")
#pragma comment(linker, "/export:_JS_ToIndex")
#pragma comment(linker, "/export:_JS_ToInt32")
#pragma comment(linker, "/export:_JS_ToInt64")
#pragma comment(linker, "/export:_JS_ToInt64Ext")
#pragma comment(linker, "/export:_JS_WriteObject2")
#pragma comment(linker, "/export:_pstrcat")
#pragma comment(linker, "/export:_pstrcpy")
#pragma comment(linker, "/export:_unicode_from_utf8")
#pragma comment(linker, "/export:_unicode_to_utf8")
#else
#pragma comment(linker, "/export:__JS_FreeValue")
#pragma comment(linker, "/export:__JS_FreeValueRT")
#pragma comment(linker, "/export:dbuf_free")
#pragma comment(linker, "/export:dbuf_init2")
#pragma comment(linker, "/export:dbuf_printf")
#pragma comment(linker, "/export:dbuf_put")
#pragma comment(linker, "/export:dbuf_putc")
#pragma comment(linker, "/export:dbuf_putstr")
#pragma comment(linker, "/export:has_suffix")
#pragma comment(linker, "/export:JS_AddModuleExport")
#pragma comment(linker, "/export:JS_AddModuleExportList")
#pragma comment(linker, "/export:JS_AtomToCString")
#pragma comment(linker, "/export:JS_Call")
#pragma comment(linker, "/export:JS_DefinePropertyValue")
#pragma comment(linker, "/export:JS_DefinePropertyValueStr")
#pragma comment(linker, "/export:JS_DefinePropertyValueUint32")
#pragma comment(linker, "/export:JS_Eval")
#pragma comment(linker, "/export:JS_EvalFunction")
#pragma comment(linker, "/export:JS_ExecutePendingJob")
#pragma comment(linker, "/export:js_free_rt")
#pragma comment(linker, "/export:js_free")
#pragma comment(linker, "/export:JS_FreeAtom")
#pragma comment(linker, "/export:JS_FreeContext")
#pragma comment(linker, "/export:JS_FreeCString")
#pragma comment(linker, "/export:JS_FreeRuntime")
#pragma comment(linker, "/export:JS_GetArrayBuffer")
#pragma comment(linker, "/export:JS_GetClassProto")
#pragma comment(linker, "/export:JS_GetException")
#pragma comment(linker, "/export:JS_GetGlobalObject")
#pragma comment(linker, "/export:JS_GetImportMeta")
#pragma comment(linker, "/export:JS_GetModuleName")
#pragma comment(linker, "/export:JS_GetOpaque")
#pragma comment(linker, "/export:JS_GetOpaque2")
#pragma comment(linker, "/export:JS_GetPropertyStr")
#pragma comment(linker, "/export:JS_GetRuntime")
#pragma comment(linker, "/export:JS_GetRuntimeOpaque")
#pragma comment(linker, "/export:JS_GetScriptOrModuleName")
#pragma comment(linker, "/export:JS_IsError")
#pragma comment(linker, "/export:JS_IsFunction")
#pragma comment(linker, "/export:JS_LoadModule")
#pragma comment(linker, "/export:js_malloc")
#pragma comment(linker, "/export:js_mallocz")
#pragma comment(linker, "/export:JS_NewArray")
#pragma comment(linker, "/export:JS_NewArrayBufferCopy")
#pragma comment(linker, "/export:JS_NewAtomLen")
#pragma comment(linker, "/export:JS_NewBigInt64")
#pragma comment(linker, "/export:JS_NewCFunction2")
#pragma comment(linker, "/export:JS_NewClass")
#pragma comment(linker, "/export:JS_NewClassID")
#pragma comment(linker, "/export:JS_NewCModule")
#pragma comment(linker, "/export:JS_NewContext")
#pragma comment(linker, "/export:JS_NewObject")
#pragma comment(linker, "/export:JS_NewObjectClass")
#pragma comment(linker, "/export:JS_NewObjectProtoClass")
#pragma comment(linker, "/export:JS_NewPromiseCapability")
#pragma comment(linker, "/export:JS_NewRuntime")
#pragma comment(linker, "/export:JS_NewString")
#pragma comment(linker, "/export:JS_NewStringLen")
#pragma comment(linker, "/export:JS_ParseJSON2")
#pragma comment(linker, "/export:JS_PromiseResult")
#pragma comment(linker, "/export:JS_PromiseState")
#pragma comment(linker, "/export:JS_ReadObject")
#pragma comment(linker, "/export:js_realloc_rt")
#pragma comment(linker, "/export:JS_ResetUncatchableError")
#pragma comment(linker, "/export:JS_ResolveModule")
#pragma comment(linker, "/export:JS_RunGC")
#pragma comment(linker, "/export:JS_SetCanBlock")
#pragma comment(linker, "/export:JS_SetClassProto")
#pragma comment(linker, "/export:JS_SetConstructor")
#pragma comment(linker, "/export:JS_SetInterruptHandler")
#pragma comment(linker, "/export:JS_SetModuleExport")
#pragma comment(linker, "/export:JS_SetModuleExportList")
#pragma comment(linker, "/export:JS_SetModuleLoaderFunc")
#pragma comment(linker, "/export:JS_SetOpaque")
#pragma comment(linker, "/export:JS_SetPropertyFunctionList")
#pragma comment(linker, "/export:JS_SetPropertyStr")
#pragma comment(linker, "/export:JS_SetPropertyUint32")
#pragma comment(linker, "/export:JS_SetRuntimeOpaque")
#pragma comment(linker, "/export:JS_SetSharedArrayBufferFunctions")
#pragma comment(linker, "/export:JS_Throw")
#pragma comment(linker, "/export:JS_ThrowOutOfMemory")
#pragma comment(linker, "/export:JS_ThrowRangeError")
#pragma comment(linker, "/export:JS_ThrowReferenceError")
#pragma comment(linker, "/export:JS_ThrowTypeError")
#pragma comment(linker, "/export:JS_ToBool")
#pragma comment(linker, "/export:JS_ToCStringLen2")
#pragma comment(linker, "/export:JS_ToFloat64")
#pragma comment(linker, "/export:JS_ToIndex")
#pragma comment(linker, "/export:JS_ToInt32")
#pragma comment(linker, "/export:JS_ToInt64")
#pragma comment(linker, "/export:JS_ToInt64Ext")
#pragma comment(linker, "/export:JS_WriteObject2")
#pragma comment(linker, "/export:pstrcat")
#pragma comment(linker, "/export:pstrcpy")
#pragma comment(linker, "/export:unicode_from_utf8")
#pragma comment(linker, "/export:unicode_to_utf8")
#endif
#endif
//==============================================================================
