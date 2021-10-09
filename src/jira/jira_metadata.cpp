#include <libstein.h>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <libstein.h>

#include "../settings.h"
#include "../util.h"
#include "jira_metadata.h"

JiraMetadata::JiraMetadata()
{
    auto &settings = Settings::get_instance();

    int position = 0;
    bool more_pages = true;

    ProgressBar progress("metadata");

    while (more_pages)
    {
        auto url = fmt::format("{}/rest/api/2/field/search?startAt={}",
            settings.jira_server,
            position);

        libstein::CachedRest response = libstein::CachedRest(url, settings.jira_user, settings.jira_key);
        progress.tick(response.is_cached());

        if (response.status_code() == 200)
        {
            auto json = nlohmann::json::parse(response.body());

            for (const auto& entry: json["values"])
            {
                auto id = entry["id"];
                auto field_name = entry["name"];
                fields_[id] = field_name;
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
