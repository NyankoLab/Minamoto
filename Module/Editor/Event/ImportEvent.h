//==============================================================================
// Minamoto : ImportEvent Header
//
// Copyright (c) 2023-2024 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <thread>
#include "Event.h"

class ImportEvent : public Event
{
    std::string title;

    std::shared_ptr<ImportEvent> thiz;
    xxNodePtr root;
    std::string name;

    std::mutex nodesMutex;
    std::vector<xxNodePtr> nodes;

    std::thread thread;
    xxNodePtr output;

public:
    ImportEvent(xxNodePtr const& root, std::string name);
    float Execute() override;

    static std::shared_ptr<ImportEvent> Create(xxNodePtr const& root, std::string name);
};
