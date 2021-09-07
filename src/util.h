#pragma once

void print_error(const std::string &error_message);

class ProgressBar
{
public:
    ProgressBar();
    ~ProgressBar();

    void tick();
};