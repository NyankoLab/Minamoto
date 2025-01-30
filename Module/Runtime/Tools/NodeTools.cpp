//==============================================================================
// Minamoto : NodeTools Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include <xxGraphicPlus/xxNode.h>
#if HAVE_MINIGUI
#include "MiniGUI/Window.h"
#endif
#include "NodeTools.h"

//==============================================================================
#if HAVE_MINIGUI
MiniGUI::WindowPtr const& NodeTools::GetRoot(MiniGUI::WindowPtr const& window)
{
    if (window == nullptr)
    {
        static MiniGUI::WindowPtr empty;
        return empty;
    }
    MiniGUI::WindowPtr const* root = nullptr;
    MiniGUI::WindowPtr const* trunk = &window;
    do
    {
        root = trunk;
        trunk = &(*trunk)->GetParent();
    } while (trunk->get() && (trunk->get()->Flags & MiniGUI::Window::WINDOW_CLASS));
    return (*root);
}
#endif
//------------------------------------------------------------------------------
xxNodePtr const& NodeTools::GetRoot(xxNodePtr const& node)
{
    if (node == nullptr)
    {
        static xxNodePtr empty;
        return empty;
    }
    xxNodePtr const* root = nullptr;
    xxNodePtr const* trunk = &node;
    do
    {
        root = trunk;
        trunk = &(*trunk)->GetParent();
    } while (trunk->get());
    return (*root);
}
//------------------------------------------------------------------------------
xxNodePtr const& NodeTools::GetObject(xxNodePtr const& node, std::string const& name)
{
    xxNodePtr const* output = nullptr;
    xxNode::Traversal(node, [&](xxNodePtr const& node)
    {
        if (node->Name == name)
            output = &node;
        return output == nullptr;
    });
    if (output == nullptr)
    {
        static xxNodePtr empty;
        return empty;
    }
    return (*output);
}
//------------------------------------------------------------------------------
xxNodePtr const& NodeTools::Intersect(xxNodePtr const& node, xxVector3 const& position, xxVector3 const& direction)
{
    float nearDistance = FLT_MAX;
    xxNodePtr const* output = nullptr;
    xxNode::Traversal(node, [&](xxNodePtr const& node)
    {
        if (node->Mesh)
        {
            float distance = (node->WorldBound.xyz - position).SquaredLength();
            if (nearDistance > distance && node->WorldBound.Intersect(position, direction))
            {
                nearDistance = distance;
                output = &node;
            }
        }
        return true;
    });

    if (output == nullptr)
    {
        static xxNodePtr empty;
        return empty;
    }
    return (*output);
}
//------------------------------------------------------------------------------
void NodeTools::ConvertQuadTree(xxNodePtr const& node)
{
    auto Bound2D = [](xxVector4 const& v)
    {
        xxVector3 o;
        o.xy = v.xy;
        o.radius = v.radius;
        return o;
    };

    std::function<void(xxNodePtr const&, xxVector3 const&, xxNodePtr const&)> attachChild = [&](xxNodePtr const& node, xxVector3 const& nodeBound, xxNodePtr const& child)
    {
        float half = nodeBound.z / 2.0f;

        xxVector3 quads[4];
        quads[0].z = half;
        quads[1].z = half;
        quads[2].z = half;
        quads[3].z = half;
        quads[0].xy = nodeBound.xy + xxVector2{  1,  1 } * half;
        quads[1].xy = nodeBound.xy + xxVector2{  1, -1 } * half;
        quads[2].xy = nodeBound.xy + xxVector2{ -1, -1 } * half;
        quads[3].xy = nodeBound.xy + xxVector2{ -1,  1 } * half;

        if (half >= 1.0f)
        {
            while (node->GetChildCount() < 4)
            {
                node->AttachChild(xxNode::Create());
            }

            xxVector3 childBound = Bound2D(child->WorldBound);
            if (0.0f < childBound.z && childBound.z <= half)
            {
                size_t index = 0;
                float nearest = FLT_MAX;
                float distances[4];
                for (size_t i = 0; i < 4; ++i)
                {
                    distances[i] = (quads[i].xy - childBound.xy).Length();
                    if (nearest > distances[i])
                    {
                        index = i;
                        nearest = distances[i];
                    }
                }
                attachChild(node->GetChild(index), quads[index], child);
                return;
            }
        }

        node->AttachChild(child);
        node->UpdateBound();
    };

    xxVector3 nodeBound = Bound2D(node->WorldBound);

    std::vector<xxNodePtr> children;
    for (xxNodePtr const& child : (*node))
    {
        children.push_back(child);
    }
    node->RemoveAllChildren();

    for (xxNodePtr const& child : children)
    {
        attachChild(node, nodeBound, child);
    }

    RemoveEmptyNode(node);

    node->UpdateBound();
}
//------------------------------------------------------------------------------
void NodeTools::RemoveEmptyNode(xxNodePtr const& node)
{
    std::vector<xxNodePtr> children;
    for (xxNodePtr const& child : (*node))
    {
        children.push_back(child);
    }
    node->RemoveAllChildren();

    for (xxNodePtr const& child : children)
    {
        if (child->Mesh || child->GetChildCount())
        {
            node->AttachChild(child);
        }
    }

    for (xxNodePtr const& child : (*node))
    {
        RemoveEmptyNode(child);
    }

    if (node->GetChildCount() == 1)
    {
        xxNodePtr child = node->GetChild(0);
        if (child->Mesh == nullptr)
        {
            node->RemoveAllChildren();

            std::vector<xxNodePtr> grandChildren;
            for (xxNodePtr const& grandChild : (*child))
            {
                grandChildren.push_back(grandChild);
            }
            child->RemoveAllChildren();

            for (xxNodePtr const& child : grandChildren)
            {
                node->AttachChild(child);
            }
        }
    }
}
//------------------------------------------------------------------------------
void NodeTools::UpdateNodeFlags(xxNodePtr const& node)
{
    xxNode::Traversal(node, [](xxNodePtr const& node)
    {
        node->Flags |= xxNode::UPDATE_SKIP;
        for (auto const& data : node->Bones)
        {
            if (data.bone.use_count())
            {
                xxNodePtr const& bone = (xxNodePtr&)data.bone;
                bone->Flags |= xxNode::UPDATE_NEED;
            }
        }
        if (node->Mesh)
        {
            node->Flags |= xxNode::UPDATE_NEED;
        }
#if HAVE_MINIGUI
        if (node->Flags & MiniGUI::Window::WINDOW_CLASS)
        {
            node->Flags |= xxNode::UPDATE_NEED;
        }
#endif
        return true;
    });

    xxNode::Traversal(node, [](xxNodePtr const& node)
    {
        if (node->Flags & xxNode::UPDATE_NEED)
        {
            xxNodePtr parent = node;
            while (parent && (parent->Flags & xxNode::UPDATE_SKIP))
            {
                parent->Flags &= ~xxNode::UPDATE_SKIP;
                parent = parent->GetParent();
            }
        }
        return true;
    });
}
//==============================================================================
