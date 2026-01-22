//==============================================================================
// Minamoto : ExportEvent Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Event/Event.h"

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
