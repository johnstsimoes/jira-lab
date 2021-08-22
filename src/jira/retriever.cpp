#include <fmt/core.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <libstein.h>

#include "retriever.h"

Retriever::Retriever (std::string user,
                      std::string token,
                      std::string host,
                      std::string jql)
{
    int position = 0;
    bool more_pages = true;

    while (more_pages)
    {
        auto url = fmt::format("{}/rest/api/2/search?jql={}&fields=key&startAt={}",
            host,
            libstein::stringutils::url_encode(jql),
            position);

        cpr::Response response = cpr::Get(cpr::Url{url},
                        cpr::Authentication{user, token});

        if (response.status_code == 200)
        {
            auto json = nlohmann::json::parse(response.text);

            for (const auto& issue : json["issues"])
            {
                results_.push_back( issue["key"]);
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
            //TODO RAII exception instead
            fmt::print("{}:{}\n", response.status_code, response.text);
            break;
        }
    }
}

std::vector<std::string> Retriever::get_results() const
{
    return results_;
}
