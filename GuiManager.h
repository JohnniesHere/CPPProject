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
    void Render();
    bool ShouldClose();
    void Cleanup();
    void SetWindowOffset(float offset) { windowOffset = offset; }

private:
    GLFWwindow* window;
    GLuint backgroundTexture;
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
    GLuint LoadTexture(const char* filename);
    void RenderBackground();
    void RenderGUI();
    void RenderDefaultWindow();
    void RenderChampionsWindow();
    GLuint championSplashTexture;
    bool isChampionSplashLoaded;
    void LoadChampionSplash(const std::string& championName);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    bool isDragging = false;
    ImVec2 dragStartPos;
    void CreateBoldFont();

};