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
    std::string autorun_filename;

    bool verbose = true;
    bool debug = false;
    bool autorun = false;
    bool localmode = true; // TODO set false as default for improved safety.

    Settings (Settings const&) = delete;
    void operator= (Settings const&) = delete;

private:
    Settings();
};
