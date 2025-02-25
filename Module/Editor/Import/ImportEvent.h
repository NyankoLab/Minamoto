//==============================================================================
// Minamoto : ImportEvent Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <set>
#include <thread>
#include "Event.h"

class ImportEvent : public Event
{
    std::shared_ptr<ImportEvent> thiz;

    std::string title;
    xxNodePtr root;
    std::string name;
    std::thread thread;

    std::mutex nodesMutex;
    std::vector<std::tuple<void*, void*, xxNodePtr, std::function<void(xxNodePtr const&)>>> nodes;
    std::map<void*, xxNodePtr> mappedNodes;
    xxNodePtr output;

    std::set<xxTexturePtr> mipmapTextures;

    int nodeCount = 0;
    int meshCount = 0;
    int textureCount = 0;

public:
    ImportEvent(xxNodePtr const& root, std::string const& name);

    double Execute() override;

    void ThreadedExecute();

    void Statistic();

    static std::shared_ptr<ImportEvent> Create(xxNodePtr const& root, std::string name);
};
