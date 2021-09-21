#pragma once

#include <map>

class JiraMetadata
{
public:
    static JiraMetadata& get_instance()
    {
        static JiraMetadata instance;
        return instance;
    }

    std::map<std::string, std::string> fields_;

    JiraMetadata (JiraMetadata const&) = delete;
    void operator= (JiraMetadata const&) = delete;

private:
    JiraMetadata();
};
