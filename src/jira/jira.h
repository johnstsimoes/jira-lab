#pragma once

#include <vector>
#include <string>

class Jira
{
    std::string jql_;
    std::vector<std::string> keys_;

public:
    Jira(const std::string &jql);

    std::vector<std::string> get_keys();
};
