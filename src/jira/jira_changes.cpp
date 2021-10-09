#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <libstein.h>
#include "../settings.h"
#include "../util.h"
#include "jira_changes.h"

JiraChanges::JiraChanges(const std::string &key, const std::string &field)
{
    auto &settings = Settings::get_instance();

    int position = 0;
    bool more_pages = true;

    ProgressBar progress;

    while (more_pages)
    {
        const auto url = fmt::format("{}/rest/api/2/issue/{}?expand=changelog&startAt={}",
            settings.jira_server,
            key,
            position);

        libstein::CachedRest response = libstein::CachedRest(url, settings.jira_user, settings.jira_key);

        if (response.status_code() == 200)
        {
            auto json = nlohmann::json::parse(response.body());
            std::string line = "";
            auto changes = json["changelog"]["histories"];
            try
            {
                for (auto& entry : changes)
                {
                    line = entry.dump();

                    auto when = jira_parse_date(entry["created"]);

                    for (auto const &item : entry["items"])
                    {
                        if (item["field"] != field)
                        {
                            continue;
                        }

                        auto new_value = item["toString"];
                        if (new_value == nullptr) new_value = "null";

                        // Add new entry to the vector
                        // when, new_value
                        JiraChangesEntry new_entry;
                        new_entry.timestamp = when;
                        new_entry.state = new_value;

                        this->results_.push_back(new_entry);
                    }
                }
            }
            catch (...)
            {
                throw std::invalid_argument(fmt::format("Could not parse response:{}", line));
            }

            int startAt = json["changelog"]["startAt"];
            int total = json["changelog"]["total"];
            int issues = json["changelog"]["histories"].size();

            if (startAt + issues >= total)
            {
                more_pages = false;
            }
            else
            {
                position += issues;
                print_warning(fmt::format("now page starting at={}", position));
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

std::vector<JiraChangesEntry> JiraChanges::get_results()
{
    return this->results_;
}
