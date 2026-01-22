//==============================================================================
// Minamoto : Console Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct Console
{
    bool UpdateInput(const UpdateData& updateData);
    bool UpdateConsole(const UpdateData& updateData, std::deque<std::string> const& lines);
    void AddHistory(const char* line);

    std::string input;

    size_t lineCount = 0;

    std::vector<std::string> history;
    size_t inputPos = 0;
    size_t historyPos = 0;
};
