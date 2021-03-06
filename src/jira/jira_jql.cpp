#include <libstein.h>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "../settings.h"
#include "../util.h"
#include "jira_jql.h"

JiraJQL::JiraJQL(const std::string &jql)
{
    auto &settings = Settings::get_instance();

    int position = 0;
    bool more_pages = true;

    ProgressBar progress;

    while (more_pages)
    {
        auto url = fmt::format("{}/rest/api/2/search?jql={}&fields=key&startAt={}",
            settings.jira_server,
            libstein::stringutils::url_encode(jql),
            position);

        libstein::CachedRest response = libstein::CachedRest(url, settings.jira_user, settings.jira_key);

        progress.tick(response.is_cached());

        if (response.status_code() == 200)
        {
            auto json = nlohmann::json::parse(response.body());

            for (const auto& issue : json["issues"])
            {
                keys_.push_back( issue["key"]);
            }

            int startAt = json["startAt"];
            int total = json["total"];
            int issues = json["issues"].size();

            if (startAt + issues >= total)
            {
                more_pages = false;
            }
            else
            {
                position += issues;
            }
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

std::vector<std::string> JiraJQL::get_keys()
{
    return this->keys_;
}
