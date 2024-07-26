#include "GUIManager.h"
#include "nlohmann/json.hpp"
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <curl/curl.h>

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

GUIManager::GUIManager()
    : window(nullptr), backgroundTexture(0), championSplashTexture(0),
    windowOffset(10.0f), currentState(WindowState::Default),
    selectedChampionIndex(-1), isChampionSplashLoaded(false) {}

GUIManager::~GUIManager() {
    Cleanup();
}

bool GUIManager::Initialize(int width, int height, const char* title) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
        return false;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ApplyCustomStyles();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    backgroundTexture = LoadTexture("D:\\CPP Project\\CPPProject\\assets\\image.png");

    if (!dataManager.FetchChampionData()) {
        std::cerr << "Failed to fetch champion data" << std::endl;
        return false;
    }

    return true;
}

void GUIManager::Render() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    RenderBackground();
    RenderGUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

bool GUIManager::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void GUIManager::Cleanup() {
    glDeleteTextures(1, &backgroundTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GUIManager::ApplyCustomStyles() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 12.0f;
    style.GrabRounding = 12.0f;
    style.FramePadding = ImVec2(10, 10);
    style.ItemSpacing = ImVec2(10, 10);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.40f, 0.59f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.52f, 0.76f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.31f, 0.50f, 1.00f);
}

GLuint GUIManager::LoadTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

void GUIManager::RenderBackground() {
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(-1, -1);
    glTexCoord2f(1, 1); glVertex2f(1, -1);
    glTexCoord2f(1, 0); glVertex2f(1, 1);
    glTexCoord2f(0, 0); glVertex2f(-1, 1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GUIManager::RenderGUI() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::Begin("Full Window", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings);

    // Render buttons
    ImGui::Text("H&J");
    ImGui::SameLine();
    if (ImGui::Button("Champions")) {
        currentState = WindowState::Champions;
        std::cout << "Champions button clicked!" << std::endl;
    }
    ImGui::SameLine();
    if (ImGui::Button("Items")) {
        currentState = WindowState::Items;
        std::cout << "Items button clicked!" << std::endl;
    }
    ImGui::SameLine();
    if (ImGui::Button("Summoner's Spells")) {
        currentState = WindowState::SummonerSpells;
        std::cout << "Summoner's Spells button clicked!" << std::endl;
    }

    buttonHeight = ImGui::GetItemRectSize().y;

    // Render the appropriate window based on the current state
    ImGui::SetCursorPos(ImVec2(0, buttonHeight + windowOffset));
    ImGui::BeginChild("Content Window", ImVec2(viewport->Size.x, viewport->Size.y - buttonHeight - windowOffset), false,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));

    switch (currentState) {
    case WindowState::Champions:
        RenderChampionsWindow();
        break;
    case WindowState::Items:
    case WindowState::SummonerSpells:
        // TODO: Implement these windows
        ImGui::Text("This feature is not implemented yet.");
        break;
    default:
        RenderDefaultWindow();
        break;
    }

    ImGui::PopStyleColor();
    ImGui::EndChild();

    ImGui::End();
}

void GUIManager::RenderDefaultWindow() {
    ImVec2 textSize = ImGui::CalcTextSize("Select one option above");
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 cursorPos(
        (windowSize.x - textSize.x) * 0.5f,
        (windowSize.y - textSize.y) * 0.5f
    );

    ImGui::SetCursorPos(cursorPos);
    ImGui::Text("Select one option above");
}

void GUIManager::RenderChampionsWindow() {
    const auto& championNames = dataManager.GetChampionNames();

    // Add drop-down menu for champion selection
    if (ImGui::BeginCombo("Select Champion", selectedChampionIndex >= 0 ? championNames[selectedChampionIndex].c_str() : "Select a champion")) {
        for (int i = 0; i < championNames.size(); i++) {
            bool is_selected = (selectedChampionIndex == i);
            if (ImGui::Selectable(championNames[i].c_str(), is_selected)) {
                selectedChampionIndex = i;
                LoadChampionSplash(championNames[i]);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Display champion splash art with transparency
    if (isChampionSplashLoaded) {
        ImVec4 tint_color = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // RGBA: 50% opacity
        ImVec4 border_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // No border
        ImGui::Image((void*)(intptr_t)championSplashTexture, ImGui::GetContentRegionAvail(), ImVec2(0, 0), ImVec2(1, 1), tint_color, border_color);
    }
}

void GUIManager::LoadChampionSplash(const std::string& championName) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string url = dataManager.GetChampionImageUrl(championName);
        std::string imageData;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            int width, height, channels;
            unsigned char* image = stbi_load_from_memory(
                reinterpret_cast<const unsigned char*>(imageData.data()),
                imageData.size(), &width, &height, &channels, 4);

            if (image) {
                if (isChampionSplashLoaded) {
                    glDeleteTextures(1, &championSplashTexture);
                }

                glGenTextures(1, &championSplashTexture);
                glBindTexture(GL_TEXTURE_2D, championSplashTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

                stbi_image_free(image);
                isChampionSplashLoaded = true;
            }
        }
    }
}

size_t GUIManager::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}