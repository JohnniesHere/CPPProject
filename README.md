# ImGui GLFW Project

This project is a C++ application using Dear ImGui and GLFW.

## Prerequisites

- Visual Studio 2022 (or compatible version)
- C++17 or later

## Setup Instructions

1. Clone or download this repository to your local machine.

2. Open the solution file (.sln) in Visual Studio.

3. The project is set up to use the following third-party libraries:
   - Dear ImGui
   - GLFW 
   - nlohmann JSON 

   These libraries are included in the `CPPProject` folder.

4. If the libraries are not detected automatically, you may need to set up the include and library directories:

   a) Right-click on the project in Solution Explorer and select "Properties"
   b) Go to C/C++ > General > Additional Include Directories
   c) Add these paths (adjust if necessary):
      - $(ProjectDir)CPPProject\imgui
      - $(ProjectDir)CPPProject\glfw-3.3.8.bin.WIN64\include
      - $(ProjectDir)CPPProject

   d) Go to Linker > General > Additional Library Directories
   e) Add this path (adjust if necessary):
      - $(ProjectDir)CPPProject\glfw-3.3.8.bin.WIN64\lib-vc2022

5. Ensure the following libraries are linked:
   - glfw3.lib
   - opengl32.lib
   - gdi32.lib
   - shell32.lib

   You can check this in Project Properties > Linker > Input > Additional Dependencies

6. Build the solution (F7 or Build > Build Solution)

7. Run the program (F5 or Debug > Start Debugging)

## Troubleshooting

- If you encounter any "cannot open source file" errors, double-check your include directories.
- For linker errors, verify that all required libraries are correctly linked.
- Make sure you're using a compatible version of Visual Studio and have the C++ desktop development workload installed.



