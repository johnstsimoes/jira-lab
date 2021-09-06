#pragma once

#include <vector>
#include <string>

class JiraComponents
{
    std::vector<std::string> components_;

public:
    JiraComponents(const std::string &project_name);

    std::vector<std::string> get_results();
};