#include <fmt/format.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "../settings.h"
#include "jira_components.h"

JiraComponents::JiraComponents(const std::string &project_name)
{
    auto &settings = Settings::get_instance();

    int position = 0;
    bool more_pages = true;

    while (more_pages)
    {
        auto url = fmt::format("{}/rest/api/2/project/{}/component?startAt={}",
            settings.jira_server,
            project_name,
            position);

        cpr::Response response = cpr::Get(cpr::Url{url},
                        cpr::Authentication{settings.jira_user, settings.jira_key});

        if (response.status_code == 200)
        {
            auto json = nlohmann::json::parse(response.text);

            for (const auto& entry : json["values"])
            {
                components_.push_back( entry["name"]);
            }

            int startAt = json["startAt"];
            int retrieved = json["values"].size();

            more_pages = !(json["isLast"]);
            position += retrieved;
        }
        else
        {
            throw std::invalid_argument(
                nlohmann::json::parse(
                    fmt::format("{{\"status_code\": {}, \"response\": {}}}", response.status_code, response.text)
                ).dump(2));
        }
    }
}

std::vector<std::string> JiraComponents::get_results()
{
    return this->components_;
}
