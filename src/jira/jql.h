#pragma once

#include <vector>
#include <string>

class JQL
{
    std::vector<std::string> keys_;

public:
    JQL(const std::string &jql);

    std::vector<std::string> get_keys();
};