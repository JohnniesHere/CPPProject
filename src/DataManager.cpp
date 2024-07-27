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

bool DataManager::FetchSpecificChampionData(const std::string& championId) const {
    if (specificChampionData.find(championId) != specificChampionData.end()) {
        return true;  // Data already fetched
    }

    auto res = client.Get(("/cdn/14.14.1/data/en_US/champion/" + championId + ".json").c_str());
    if (res && res->status == 200) {
        specificChampionData[championId] = nlohmann::json::parse(res->body);
        return true;
    }
    std::cerr << "Failed to fetch specific champion data for " << championId << std::endl;
    return false;
}


void DataManager::ProcessChampionData() {
    championNames.clear();
    nameToIdMap.clear();
    for (auto& [key, value] : championData["data"].items()) {
        std::string name = value["name"];
        championNames.push_back(name);
        nameToIdMap[name] = key;
    }
}

const std::vector<std::string>& DataManager::GetChampionNames() const {
    return championNames;
}

std::string DataManager::GetChampionId(const std::string& championName) const {
    auto it = nameToIdMap.find(championName);
    if (it != nameToIdMap.end()) {
        return it->second;
    }
    return championName; // Fallback to the name if ID is not found
}

std::string DataManager::GetChampionImageUrl(const std::string& championId) const {
    return "https://ddragon.leagueoflegends.com/cdn/img/champion/splash/" + championId + "_0.jpg";
}

std::string DataManager::GetChampionIconUrl(const std::string& championId) const {
    return "http://ddragon.leagueoflegends.com/cdn/14.14.1/img/champion/" + championId + ".png";
}

nlohmann::json DataManager::GetChampionStats(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["stats"];
}

std::string DataManager::GetChampionTitle(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    return championData["data"][championId]["title"];
}

std::string DataManager::GetChampionLore(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["lore"];
}

std::vector<std::string> DataManager::GetChampionTags(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    return championData["data"][championId]["tags"].get<std::vector<std::string>>();
}

nlohmann::json DataManager::GetChampionSpells(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["spells"];
}

nlohmann::json DataManager::GetChampionPassive(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["passive"];
}

nlohmann::json DataManager::GetChampionData(const std::string& championId) const {
    return championData["data"][championId];
}

nlohmann::json DataManager::GetChampionSkins(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["skins"];
}

std::string DataManager::GetChampionSkinImageUrl(const std::string& championId, const std::string& skinNum) const {
    return "https://ddragon.leagueoflegends.com/cdn/img/champion/splash/" + championId + "_" + skinNum + ".jpg";
}

std::vector<std::string> DataManager::GetChampionAllyTips(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["allytips"].get<std::vector<std::string>>();
}

std::vector<std::string> DataManager::GetChampionEnemyTips(const std::string& championName) const {
    std::string championId = GetChampionId(championName);
    FetchSpecificChampionData(championId);
    return specificChampionData.at(championId)["data"][championId]["enemytips"].get<std::vector<std::string>>();
}