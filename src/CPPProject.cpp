#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <json.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

GLuint loadTexture(const char* filename)
{
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

void ApplyCustomStyles()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Customize style for buttons
    style.FrameRounding = 12.0f;
    style.GrabRounding = 12.0f;
    style.FramePadding = ImVec2(10, 10);
    style.ItemSpacing = ImVec2(10, 10);

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.40f, 0.59f, 1.00f); // Button color
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.52f, 0.76f, 1.00f); // Button color when hovered
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.31f, 0.50f, 1.00f); // Button color when pressed
}

int main()
{
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ApplyCustomStyles(); // Apply custom styles

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Load background texture
    GLuint backgroundTexture = loadTexture("D:\\CPP Project\\CPPProject\\assets\\image.png");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render background
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

        // Create a full-window frame
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);

            ImGui::Begin("Full Window", nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings);

            // Add buttons in a single line
            ImGui::Text("H&J");
            ImGui::SameLine();
            if (ImGui::Button("Champions"))
                std::cout << "Champions button clicked!" << std::endl;
            ImGui::SameLine();
            if (ImGui::Button("Items"))
                std::cout << "Items button clicked!" << std::endl;
            ImGui::SameLine();
            if (ImGui::Button("Summoner's Spells"))
                std::cout << "Summoner's Spells button clicked!" << std::endl;

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &backgroundTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
