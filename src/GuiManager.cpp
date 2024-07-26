#include "GUIManager.h"
#include "nlohmann/json.hpp"
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <curl/curl.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <misc/freetype/imgui_freetype.h>

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

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
        return false;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    windowSize = ImVec2(width, height);

    // Set the resize callback
    glfwSetWindowSizeCallback(window, WindowResizeCallback);
    /*
    HWND hwnd = glfwGetWin32Window(window);
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    style |= WS_POPUP;
    SetWindowLong(hwnd, GWL_STYLE, style);

    HRGN hRgn = CreateRoundRectRgn(0, 0, width, height, 20, 20);
    SetWindowRgn(hwnd, hRgn, TRUE);
    */

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

    // Handle window dragging
    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        isDragging = true;
        dragStartPos = ImGui::GetMousePos();
    }
    else if (ImGui::IsMouseReleased(0))
    {
        isDragging = false;
    }

    if (isDragging)
    {
        ImVec2 delta = ImVec2(ImGui::GetMousePos().x - dragStartPos.x, ImGui::GetMousePos().y - dragStartPos.y);

        int x, y;
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowPos(window, x + delta.x, y + delta.y);
        dragStartPos = ImGui::GetMousePos();
    }

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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Full Window", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus);

    // Handle dragging
    if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
    {
        isDragging = true;
        dragStartPos = ImGui::GetMousePos();
    }
    else if (ImGui::IsMouseReleased(0))
    {
        isDragging = false;
    }

    if (isDragging)
    {
        ImVec2 delta = ImVec2(ImGui::GetMousePos().x - dragStartPos.x, ImGui::GetMousePos().y - dragStartPos.y);
        int x, y;
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowPos(window, x + delta.x, y + delta.y);
        dragStartPos = ImGui::GetMousePos();
    }

    // Add a small resize handle in the bottom-right corner
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 resizeHandlePos = ImVec2(windowPos.x + windowSize.x - 20, windowPos.y + windowSize.y - 20);
    ImGui::GetWindowDrawList()->AddTriangleFilled(
        ImVec2(resizeHandlePos.x + 20, resizeHandlePos.y + 20),
        ImVec2(resizeHandlePos.x + 20, resizeHandlePos.y),
        ImVec2(resizeHandlePos.x, resizeHandlePos.y + 20),
        IM_COL32(200, 200, 200, 255));

    // Handle resizing
    if (ImGui::IsMouseHoveringRect(resizeHandlePos, ImVec2(resizeHandlePos.x + 20, resizeHandlePos.y + 20)))
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        if (ImGui::IsMouseClicked(0))
        {
            isResizing = true;
            resizeStartPos = ImGui::GetMousePos();
        }
    }

    if (ImGui::IsMouseReleased(0))
    {
        isResizing = false;
    }

    if (isResizing)
    {
        ImVec2 delta = ImVec2(ImGui::GetMousePos().x - resizeStartPos.x, ImGui::GetMousePos().y - resizeStartPos.y);
        windowSize.x += delta.x;
        windowSize.y += delta.y;
        glfwSetWindowSize(window, windowSize.x, windowSize.y);
        resizeStartPos = ImGui::GetMousePos();
    }

    // Add custom close and minimize buttons
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 60, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    if (ImGui::Button("--", ImVec2(25, 25)))
    {
        glfwIconifyWindow(window);
    }
    ImGui::SameLine();
    if (ImGui::Button("X", ImVec2(25, 25)))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    // Render buttons
    ImGui::SetCursorPos(ImVec2(10, 10));  // Adjust position as needed
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
    ImGui::PopStyleVar(2);
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

    // Champion selection dropdown
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
    if (ImGui::BeginCombo("##ChampionSelect", selectedChampionIndex >= 0 ? championNames[selectedChampionIndex].c_str() : "Select Champion")) {
        for (int i = 0; i < championNames.size(); i++) {
            bool is_selected = (selectedChampionIndex == i);
            if (ImGui::Selectable(championNames[i].c_str(), is_selected)) {
                selectedChampionIndex = i;
                LoadChampionSplash(championNames[i]);
                LoadChampionIcon(championNames[i]);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Display champion splash art as background
    if (isChampionSplashLoaded && selectedChampionIndex >= 0) {
        ImGui::GetWindowDrawList()->AddImage(
            (void*)(intptr_t)championSplashTexture,
            ImGui::GetWindowPos(),
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
            ImVec2(0, 0), ImVec2(1, 1),
            IM_COL32(255, 255, 255, 128)  // 50% opacity
        );
    }

    if (selectedChampionIndex >= 0) {
        std::string championName = championNames[selectedChampionIndex];

        // Display champion icon
        if (isChampionIconLoaded) {
            ImGui::SetCursorPos(ImVec2(10, 50));
            ImGui::Image((void*)(intptr_t)championIconTexture, ImVec2(64, 64));
        }

        // Display champion stats
        ImGui::SetCursorPos(ImVec2(10, 120));
        ImGui::BeginChild("ChampionStats", ImVec2(300, 300), true, ImGuiWindowFlags_NoScrollbar);

        nlohmann::json stats = dataManager.GetChampionStats(championName);

        ImGui::Text("Champion: %s", championName.c_str());
        ImGui::Text("Base Stats:");
        ImGui::Text("HP: %.0f (+ %.0f per level)", stats["hp"].get<float>(), stats["hpperlevel"].get<float>());
        ImGui::Text("Armor: %.1f (+ %.2f per level)", stats["armor"].get<float>(), stats["armorperlevel"].get<float>());
        ImGui::Text("Magic Resist: %.1f (+ %.2f per level)", stats["spellblock"].get<float>(), stats["spellblockperlevel"].get<float>());
        ImGui::Text("Move Speed: %.0f", stats["movespeed"].get<float>());
        ImGui::Text("Attack Damage: %.0f (+ %.0f per level)", stats["attackdamage"].get<float>(), stats["attackdamageperlevel"].get<float>());
        ImGui::Text("Attack Speed: %.3f (+ %.1f%% per level)", stats["attackspeed"].get<float>(), stats["attackspeedperlevel"].get<float>());
        ImGui::Text("Attack Range: %.0f", stats["attackrange"].get<float>());
        ImGui::Text("HP Regen: %.1f (+ %.1f per level)", stats["hpregen"].get<float>(), stats["hpregenperlevel"].get<float>());

        ImGui::EndChild();
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

void GUIManager::LoadChampionIcon(const std::string& championName) {
    // Similar to LoadChampionSplash, but for the icon
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string url = dataManager.GetChampionIconUrl(championName);  // You'll need to add this function to DataManager
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
                if (isChampionIconLoaded) {
                    glDeleteTextures(1, &championIconTexture);
                }

                glGenTextures(1, &championIconTexture);
                glBindTexture(GL_TEXTURE_2D, championIconTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

                stbi_image_free(image);
                isChampionIconLoaded = true;
            }
        }
    }
}

size_t GUIManager::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void GUIManager::CreateBoldFont() {
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.MergeMode = false;
    config.PixelSnapH = true;
    config.OversampleH = 1;
    config.OversampleV = 1;
    config.GlyphExtraSpacing.x = 1.0f;
    config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Bold;

    // Load a bold font (you may need to adjust the path)
    io.Fonts->AddFontFromFileTTF("path/to/your/bold/font.ttf", 18.0f, &config);
}

void GUIManager::WindowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}