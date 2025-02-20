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
    std::vector<xxNodePtr> nodes;
    xxNodePtr output;

    std::set<xxTexturePtr> mipmapTextures;

    int nodeCount = 0;
    int meshCount = 0;
    int textureCount = 0;

public:
    ImportEvent(xxNodePtr const& root, std::string const& name);

    float Execute() override;

    void ThreadedExecute();

    void Statistic();

    static std::shared_ptr<ImportEvent> Create(xxNodePtr const& root, std::string name);
};
