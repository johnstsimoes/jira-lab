#include <string>
#include <fmt/color.h>

#include "util.h"

void print_error(const std::string &error_message)
{
    fmt::print(fg(fmt::color::red), "{}\n", error_message);
}
