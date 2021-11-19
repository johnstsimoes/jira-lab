#include <string>
#include <fmt/color.h>
#include <iostream>

#include "util.h"
#include "settings.h"

void print_warning(const std::string &warning_message)
{
    fmt::print(fg(fmt::color::salmon), "{}\n", warning_message);
}

void print_error(const std::string &error_message)
{
    fmt::print(fg(fmt::color::red), "{}\n", error_message);
}

void print_successfully_loaded(int amount, const std::string &what)
{
    if (Settings::get_instance().verbose)
        fmt::print(fg(fmt::color::light_green), "Successfully loaded {} {}.\n", amount, what);
}

ProgressBar::ProgressBar(std::string what)
{
    if (Settings::get_instance().verbose)
    {
        if (what.length() > 0)
            what.insert(0, " ");

        printf("\e[?25l"); // Hide the cursor
        fmt::print(fg(fmt::color::dark_green), "Loading{}:", what);
    }
}

ProgressBar::~ProgressBar()
{
    if (Settings::get_instance().verbose)
    {
        printf("\e[?25h");
        fmt::print("\n");
    }
}

void ProgressBar::tick(bool is_cached)
{

    if (Settings::get_instance().verbose)
    {
        auto color = is_cached ?
            fg(fmt::color::green) :
            fg(fmt::color::dark_green);

        fmt::print(color, ".");
        std::cout << std::flush;
    }
}

const std::tm jira_parse_date(const std::string& date)
{
    std::tm parsed = {0};

    if (!strptime(date.c_str(), "%Y-%m-%dT%H:%M:%S", &parsed))
    {
        throw std::runtime_error("failed to parse time string");
    }

    // Let system decide - not recommended usually.
    parsed.tm_isdst = -1;

    return parsed;
}
