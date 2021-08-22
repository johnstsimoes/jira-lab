#include "settings.h"

#include <libstein.h>

Settings::Settings()
{
    jira_user = libstein::stringutils::get_environment("JIRA_USER");
    jira_key = libstein::stringutils::get_environment("JIRA_TOKEN");
    jira_server = libstein::stringutils::get_environment("JIRA_HOST");
}