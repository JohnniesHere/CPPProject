#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include "DataManager.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <random>
#include <algorithm>

class GUIManager {
public:
    GUIManager();
    ~GUIManager();

    bool Initialize(int width, int height, const char* title);
    bool ShouldClose();
    void Render();
    void Cleanup();
    void SetWindowOffset(float offset) { windowOffset = offset; }

private:
    GLFWwindow* window;
    GLuint backgroundTexture;
    GLuint LoadTexture(const char* filename);
    GLuint championSplashTexture;
    GLuint championIconTexture;


    float buttonHeight;
    float windowOffset;

    enum class WindowState {
        Default,
        Champions,
        Items,
        SummonerSpells
    };
    WindowState currentState;

    DataManager dataManager;  // Make sure this is declared only once
    int selectedChampionIndex;

    void ApplyCustomStyles();
    
    void RenderBackground();
    void RenderGUI();
    void RenderDefaultWindow();
    void RenderChampionsWindow();

    void CleanupSkinTextures();

    bool isChampionSplashLoaded;
    bool LoadIconTexture(const char* filename);
    void LoadSkillIcons(const std::string& championId);
    void LoadSkillIcon(const std::string& iconFilename, int index);
    GLuint LoadSkinTexture(const std::string& url);
    void RandomizeTips(const std::vector<std::string>& tips, std::vector<size_t>& indices);
    bool isChampionIconLoaded;
    void LoadChampionSplash(const std::string& championName);
    void LoadChampionIcon(const std::string& championName);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    bool isDragging = false;
    ImVec2 dragStartPos;
    bool isResizing = false;
    ImVec2 resizeStartPos;
    ImVec2 windowSize;
    void CreateBoldFont();
    static void WindowResizeCallback(GLFWwindow* window, int width, int height);
    GLuint iconTexture;
    bool isIconLoaded;
    std::vector<GLuint> skillTextures;
    bool areSkillIconsLoaded;
    std::string selectedSkill;
    std::string skillDescription;
    std::map<std::string, std::string> skillDescriptions;
    bool showSkins = false;
    size_t currentSkinIndex = 0;
    std::map<std::string, GLuint> skinTextures;
    std::vector<std::string> allyTips;
    std::vector<std::string> enemyTips;
    std::vector<size_t> tipIndices;
    size_t currentTipIndex;
    std::mutex tipMutex;
    bool showAllyTip = false;
    std::mt19937 rng;  // Random number generator
    bool showEnemyTip = false;
    std::vector<size_t> allyTipIndices;
    std::vector<size_t> enemyTipIndices;
    size_t currentAllyTipIndex = 0;
    size_t currentEnemyTipIndex = 0;
    
};