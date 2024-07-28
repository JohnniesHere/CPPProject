#pragma once

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <httplib.h>

class DataManager {
public:
    DataManager();

    bool FetchChampionData();
    bool FetchSpecificChampionData(const std::string& championId) const;  // Now const
    const std::vector<std::string>& GetChampionNames() const;
    std::string GetChampionId(const std::string& championName) const;
    std::string GetChampionImageUrl(const std::string& championId) const;
    std::string GetChampionIconUrl(const std::string& championId) const;
    nlohmann::json GetChampionStats(const std::string& championName) const;
    std::string GetChampionTitle(const std::string& championName) const;
    std::string GetChampionLore(const std::string& championName) const;
    std::vector<std::string> GetChampionTags(const std::string& championName) const;
    nlohmann::json GetChampionSpells(const std::string& championName) const;
    nlohmann::json GetChampionPassive(const std::string& championName) const;
    nlohmann::json GetChampionData(const std::string& championId) const;
    nlohmann::json GetChampionSkins(const std::string& championName) const;
    std::string GetChampionSkinImageUrl(const std::string& championId, const std::string& skinNum) const;
    std::vector<std::string> GetChampionAllyTips(const std::string& championName) const;
    std::vector<std::string> GetChampionEnemyTips(const std::string& championName) const;

private:
    mutable httplib::Client client;
    nlohmann::json championData;
    mutable std::map<std::string, nlohmann::json> specificChampionData;  // Now mutable
    std::vector<std::string> championNames;
    std::map<std::string, std::string> nameToIdMap;

    void ProcessChampionData();
};