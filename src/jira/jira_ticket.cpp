#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <libstein.h>
#include "../settings.h"
#include "../util.h"
#include "jira_ticket.h"

JiraTicket::JiraTicket(const std::string &key)
{
    auto &settings = Settings::get_instance();

    const auto url = fmt::format("{}/rest/api/2/issue/{}",
        settings.jira_server,
        key);

    libstein::CachedRest response = libstein::CachedRest(url, settings.jira_user, settings.jira_key);

    if (response.status_code() == 200)
    {
        auto json = nlohmann::json::parse(response.body());

        auto fields = json["fields"];

        for (auto& entry : fields.items())
        {
            this->fields_[entry.key()] = entry.value().dump(2);
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

std::map<std::string, std::string> JiraTicket::get_fields()
{
    return this->fields_;
}
