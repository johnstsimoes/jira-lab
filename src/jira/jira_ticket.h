#pragma once

#include <map>
#include <string>

class JiraTicket
{
    std::map<std::string, std::string> fields_;

public:
    JiraTicket(const std::string &key);

    std::map<std::string, std::string> get_fields();
};