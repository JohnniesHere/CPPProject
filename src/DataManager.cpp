#include "DataManager.h"
#include <iostream>

DataManager::DataManager() : client("ddragon.leagueoflegends.com"), itemClient("cdn.merakianalytics.com") {}

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
    championNameToIdMap.clear();
    for (auto& [key, value] : championData["data"].items()) {
        std::string name = value["name"];
        championNames.push_back(name);
        championNameToIdMap[name] = key;
    }
}

const std::vector<std::string>& DataManager::GetChampionNames() const {
    return championNames;
}

std::string DataManager::GetChampionId(const std::string& championName) const {
    auto it = championNameToIdMap.find(championName);
    if (it != championNameToIdMap.end()) {
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

// item window functions
bool DataManager::FetchItemData() {
    auto res = itemClient.Get("/riot/lol/resources/latest/en-US/items.json");
    if (res && res->status == 200) {
        itemData = nlohmann::json::parse(res->body);
        ProcessItemData();
        return true;
    }
    std::cerr << "Failed to fetch item data" << std::endl;
    return false;
}
bool DataManager::FetchSpecificItemData(const std::string& itemId) const {
    // Check if we already have this item's data
    if (specificItemData.find(itemId) != specificItemData.end()) {
        return true;  // Data already fetched
    }
    // Check if we have already loaded the entire item data
    if (itemData.empty()) {
        // If not, fetch the entire item data
        auto res = itemClient.Get("/riot/lol/resources/latest/en-US/items.json");
        if (!res || res->status != 200) {
            std::cerr << "Failed to fetch item data" << std::endl;
            return false;
        }
        itemData = nlohmann::json::parse(res->body);
    }
    // Search for the specific item in the itemData
    auto it = itemData.find(itemId);
    if (it != itemData.end()) {
        // Store the specific item data
        specificItemData[itemId] = *it;
        // Update itemNames and itemNameToIdMap if not already present
        std::string itemName = (*it)["name"];
        if (std::find(itemNames.begin(), itemNames.end(), itemName) == itemNames.end()) {
            itemNames.push_back(itemName);
            itemNameToIdMap[itemName] = itemId;
        }
        return true;
    }
    else {
        std::cerr << "Item ID " << itemId << " not found in item data" << std::endl;
        return false;
    }
}
void DataManager::ProcessItemData() {
    itemNames.clear();
    itemNameToIdMap.clear();
    for (auto& [key, value] : itemData["data"].items()) {
        std::string name = value["name"];
        itemNames.push_back(name);
        itemNameToIdMap[name] = key;
    }
}
const std::vector<std::string>& DataManager::GetItemNames() const {
    return itemNames;
}
std::string DataManager::GetItemId(const std::string& itemName) const {
    auto it = itemNameToIdMap.find(itemName);
    if (it != itemNameToIdMap.end()) {
        return it->second;
    }
    return itemName; // Fallback to the name if ID is not found
}
std::string DataManager::GetSpecificItemName(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    return specificItemData.at(itemId)[itemId]["name"];
}
std::string DataManager::GetItemDescription(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    return specificItemData.at(itemId)[itemId]["simpleDescription"];
}
std::vector<std::string> DataManager::GetItemBuildsFrom(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    std::vector<std::string> buildsFrom;
    if (specificItemData.at(itemId).contains("buildsFrom")) {
        for (const auto& item : specificItemData.at(itemId)["buildsFrom"]) {
            buildsFrom.push_back(item.get<std::string>());
        }
    }
    return buildsFrom;
}
std::vector<std::string> DataManager::GetItemBuildsInto(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    std::vector<std::string> buildsInto;
    if (specificItemData.at(itemId).contains("buildsInto")) {
        for (const auto& item : specificItemData.at(itemId)["buildsInto"]) {
            buildsInto.push_back(item.get<std::string>());
        }
    }
    return buildsInto;
}
std::string DataManager::GetItemImageUrl(const std::string& itemId) const {
    return "https://ddragon.leagueoflegends.com/cdn/14.14.1/img/item/" + itemId + ".png";
}
int DataManager::GetItemCost(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    if (specificItemData.at(itemId).contains("shop") && specificItemData.at(itemId)["shop"].contains("prices")) {
        return specificItemData.at(itemId)["shop"]["prices"]["total"].get<int>();
    }
    return -1;  // Return a default value or handle the case where the item cost is not available
}
int DataManager::GetItemSellPrice(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    if (specificItemData.at(itemId).contains("shop") && specificItemData.at(itemId)["shop"].contains("prices")) {
        return specificItemData.at(itemId)["shop"]["prices"]["sell"].get<int>();
    }
    return -1;  // Return a default value or handle the case where the sell price is not available
}
bool DataManager::IsItemPurchasable(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    if (specificItemData.at(itemId).contains("shop")) {
        return specificItemData.at(itemId)["shop"]["purchasable"].get<bool>();
    }
    return false;  // Default value if the key is not found
}
std::vector<std::string> DataManager::GetItemTags(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    std::vector<std::string> tags;
    if (specificItemData.at(itemId).contains("shop") && specificItemData.at(itemId)["shop"].contains("tags")) {
        for (const auto& tag : specificItemData.at(itemId)["shop"]["tags"]) {
            tags.push_back(tag.get<std::string>());
        }
    }
    return tags;
}
nlohmann::json DataManager::GetItemStats(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    if (specificItemData.at(itemId).contains("stats")) {
        return specificItemData.at(itemId)["stats"];
    }
    return {};  // Return an empty JSON object if the key is not found
}
nlohmann::json DataManager::GetItemData(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    if (specificItemData.at(itemId).contains(itemId)) {
        return specificItemData.at(itemId);
    }
    return {};  // Return an empty JSON object if the item is not found
}
nlohmann::json DataManager::GetItemShopInfo(const std::string& itemId) const {
    FetchSpecificItemData(itemId);
    if (specificItemData.at(itemId).contains("shop")) {
        return specificItemData.at(itemId)["shop"];
    }
    return {};  // Return an empty JSON object if the shop info is not found
}