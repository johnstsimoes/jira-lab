#pragma once

#include <string>
#include <ctime>

void print_error(const std::string &error_message);
void print_warning(const std::string &warning_message);
void print_successfully_loaded(int amount, const std::string &what);

std::string color_highlight(const std::string &message);

class ProgressBar
{
public:
    ProgressBar(std::string what = "");
    ~ProgressBar();

    void tick(bool is_cached = false);
};

const std::tm jira_parse_date(const std::string& date);
