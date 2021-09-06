#pragma once

#include <vector>
#include <string>

class JiraJQL
{
    std::vector<std::string> keys_;

public:
    JiraJQL(const std::string &jql);

    std::vector<std::string> get_keys();
};