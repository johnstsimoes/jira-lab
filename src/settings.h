#pragma once

#include <string>

class Settings
{
public:
    static Settings& get_instance()
    {
        static Settings instance;
        return instance;
    }

    std::string jira_user;
    std::string jira_key;
    std::string jira_server;

    bool verbose = true;

    Settings (Settings const&) = delete;
    void operator= (Settings const&) = delete;

private:
    Settings();
};
