//==============================================================================
// Minamoto : ExportEvent Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Event.h"

namespace IGFD { class FileDialog; }

class ExportEvent : public Event
{
    std::string title;
    xxNodePtr root;
    std::string path;
    std::string name;

    bool pathItemWidth = false;
    IGFD::FileDialog* fileDialog = nullptr;

public:
    ExportEvent(xxNodePtr const& root);
    virtual ~ExportEvent();

    double Execute() override;

    void Filmbox();
    void Minamoto();
    void Wavefront();

    static std::shared_ptr<ExportEvent> Create(xxNodePtr const& root);
};
