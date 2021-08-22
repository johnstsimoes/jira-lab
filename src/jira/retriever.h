#pragma once

#include <string>
#include <vector>

class Retriever
{
private:
    std::vector<std::string> results_;

public:
    explicit Retriever (std::string user,
               std::string token,
               std::string host,
               std::string jql);

    std::vector<std::string> get_results() const;
};
