//==============================================================================
// Minamoto : Node Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include "Material.h"
#include "Mesh.h"
#include "Node.h"

//==============================================================================
//  Node redirect
//==============================================================================
#if defined(xxMACOS) || defined(xxIOS)
extern "C" bool _ZN6xxNode11AttachChildERKNSt3__110shared_ptrIS_EE(Node* thiz, xxNodePtr const& child);
extern "C" bool _ZN6xxNode11DetachChildERKNSt3__110shared_ptrIS_EE(Node* thiz, xxNodePtr const& child);
extern "C" bool _ZN6xxNode9TraversalERKNSt3__110shared_ptrIS_EENS0_8functionIFbS4_EEE(xxNodePtr const& node, std::function<bool(xxNodePtr const&)> callback);
#endif
//------------------------------------------------------------------------------
bool Node::AttachChild(xxNodePtr const& child)
{
#if defined(xxMACOS) || defined(xxIOS)
    return _ZN6xxNode11AttachChildERKNSt3__110shared_ptrIS_EE(this, child);
#else
    return xxNode::AttachChild(child);
#endif
}
//------------------------------------------------------------------------------
bool Node::DetachChild(xxNodePtr const& child)
{
#if defined(xxMACOS) || defined(xxIOS)
    return _ZN6xxNode11DetachChildERKNSt3__110shared_ptrIS_EE(this, child);
#else
    return xxNode::DetachChild(child);
#endif
}
//------------------------------------------------------------------------------
bool Node::Traversal(xxNodePtr const& node, std::function<bool(xxNodePtr const&)> callback)
{
#if defined(xxMACOS) || defined(xxIOS)
    return _ZN6xxNode9TraversalERKNSt3__110shared_ptrIS_EENS0_8functionIFbS4_EEE(node, callback);
#else
    return xxNode::Traversal(node, callback);
#endif
}
//==============================================================================
//  Node
//==============================================================================
static xxNodePtr (*backupBinaryCreate)();
//------------------------------------------------------------------------------
void Node::Initialize()
{
    if (backupBinaryCreate)
        return;
    backupBinaryCreate = xxNode::BinaryCreate;

    xxNode::BinaryCreate = []() -> xxNodePtr
    {
        xxNodePtr node = xxNodePtr(new Node(), [](Node* node) { delete node; });
        if (node == nullptr)
            return nullptr;

        node->m_this = node;
        return node;
    };
}
//------------------------------------------------------------------------------
void Node::Shutdown()
{
    if (backupBinaryCreate == nullptr)
        return;
    xxNode::BinaryCreate = backupBinaryCreate;
    backupBinaryCreate = nullptr;
}
//------------------------------------------------------------------------------
