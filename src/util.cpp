#include <string>
#include <fmt/color.h>
#include <iostream>

#include "util.h"

void print_error(const std::string &error_message)
{
    fmt::print(fg(fmt::color::red), "{}\n", error_message);
}

ProgressBar::ProgressBar()
{
    printf("\e[?25l"); // Hide the cursor
    fmt::print(fg(fmt::color::dark_green), "Loading:");
}

ProgressBar::~ProgressBar()
{
    printf("\e[?25h");
    fmt::print("\n");
}

void ProgressBar::tick()
{
    fmt::print(fg(fmt::color::dark_green), ".");
    std::cout << std::flush;
}