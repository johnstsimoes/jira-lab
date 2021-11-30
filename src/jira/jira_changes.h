#pragma once

#include <vector>
#include <string>

struct JiraChangesEntry
{
    std::tm timestamp;
    std::string value;
    std::string old_value;
};

class JiraChanges
{
    std::vector<JiraChangesEntry> results_;

public:
    JiraChanges(const std::string &key, const std::string &field);

    std::vector<JiraChangesEntry> get_results();
};