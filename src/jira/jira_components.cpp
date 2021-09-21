#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <libstein.h>

#include "../settings.h"
#include "../util.h"
#include "jira_components.h"

JiraComponents::JiraComponents(const std::string &project_name)
{
    auto &settings = Settings::get_instance();

    int position = 0;
    bool more_pages = true;

    ProgressBar progress;

    while (more_pages)
    {
        progress.tick();

        auto url = fmt::format("{}/rest/api/2/project/{}/component?startAt={}",
            settings.jira_server,
            project_name,
            position);

        libstein::CachedRest response = libstein::CachedRest(url, settings.jira_user, settings.jira_key);

        if (response.status_code() == 200)
        {
            auto json = nlohmann::json::parse(response.body());

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
                    fmt::format("{{\"status_code\": {}, \"response\": {}}}", response.status_code(), response.body())
                ).dump(2));
        }
    }
}

std::vector<std::string> JiraComponents::get_results()
{
    return this->components_;
}
