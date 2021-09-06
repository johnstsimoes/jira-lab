#include "jira.h"

#include "retriever.h"
#include "../settings.h"

Jira::Jira(const std::string &jql)
{
    auto &settings = Settings::get_instance();
    Retriever retriever (settings.jira_user,
                         settings.jira_key,
                         settings.jira_server,
                         jql);

    this->jql_ = jql;
    this->keys_ = retriever.get_results();
}

std::vector<std::string> Jira::get_keys()
{
    return this->keys_;
}
