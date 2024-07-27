#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include "DataManager.h"

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

    bool isChampionSplashLoaded;
    bool LoadIconTexture(const char* filename);
    void LoadSkillIcons(const std::string& championId);
    void LoadSkillIcon(const std::string& iconFilename, int index);
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

};