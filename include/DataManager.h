#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <httplib.h>

class DataManager {
public:
    DataManager();
    bool FetchChampionData();
    const std::vector<std::string>& GetChampionNames() const;
    std::string GetChampionImageUrl(const std::string& championName) const;

private:
    httplib::Client client;
    nlohmann::json championData;
    std::vector<std::string> championNames;

    void ProcessChampionData();
};