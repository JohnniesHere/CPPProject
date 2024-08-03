
#include "../GuiManager.h"

int main()
{
    GUIManager gui;

    if (!gui.Initialize(1280, 720, "Dear ImGui GLFW+OpenGL3 example"))
        return 1;
    gui.SetWindowOffset(20.0f);  // Set a 20-pixel offset

    while (!gui.ShouldClose())
    {
        
        gui.Render();
        
    }

    return 0;
}