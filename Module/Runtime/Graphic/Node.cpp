//==============================================================================
// Minamoto : Node Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
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
extern "C" bool _ZN6xxNode9TraversalERKNSt3__110shared_ptrIS_EERKNS0_8functionIFiS4_EEE(xxNodePtr const& node, std::function<int(xxNodePtr const&)> const& callback);
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
void Node::Invalidate()
{
    for (auto& constantData : ConstantDatas)
    {
        xxDestroyPipeline(constantData.pipeline);
        xxDestroyShader(constantData.device, constantData.meshShader);
        xxDestroyShader(constantData.device, constantData.vertexShader);
        xxDestroyShader(constantData.device, constantData.fragmentShader);
        constantData.pipeline = 0;
        constantData.meshShader = 0;
        constantData.vertexShader = 0;
        constantData.fragmentShader = 0;
    }
    return xxNode::Invalidate();
}
//------------------------------------------------------------------------------
void Node::Draw(xxDrawData const& data)
{
    if (Mesh == nullptr)
        return;
    xxMaterialPtr const& material = Material ? Material : Material::DefaultMaterial;

    data.mesh = Mesh.get();
    data.node = this;

    Mesh->Setup(data.device);
    material->Setup(data);

    if (data.constantData->ready <= 0)
        return;

    material->Draw(data);
    Mesh->Draw(data.commandEncoder);
}
//------------------------------------------------------------------------------
bool Node::Traversal(xxNodePtr const& node, std::function<int(xxNodePtr const&)> const& callback)
{
#if defined(xxMACOS) || defined(xxIOS)
    return _ZN6xxNode9TraversalERKNSt3__110shared_ptrIS_EERKNS0_8functionIFiS4_EEE(node, callback);
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
