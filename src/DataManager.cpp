#include "DataManager.h"
#include <iostream>

DataManager::DataManager() : client("ddragon.leagueoflegends.com") {}

bool DataManager::FetchChampionData() {
    auto res = client.Get("/cdn/14.14.1/data/en_US/champion.json");
    if (res && res->status == 200) {
        championData = nlohmann::json::parse(res->body);
        ProcessChampionData();
        return true;
    }
    std::cerr << "Failed to fetch champion data" << std::endl;
    return false;
}

void DataManager::ProcessChampionData() {
    championNames.clear();
    for (auto& [key, value] : championData["data"].items()) {
        championNames.push_back(value["name"]);
    }
}

const std::vector<std::string>& DataManager::GetChampionNames() const {
    return championNames;
}

std::string DataManager::GetChampionImageUrl(const std::string& championName) const {
    return "https://ddragon.leagueoflegends.com/cdn/img/champion/splash/" + championName + "_0.jpg";
}