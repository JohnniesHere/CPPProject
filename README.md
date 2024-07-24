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

## Collaborating via GitHub

To collaborate on this project using GitHub, follow these steps:


1. Access to the Repository:
   - The project owner will need to add you as a collaborator.
   - Check your email for an invitation to collaborate and accept it.

2. Clone the Repository:
   - Open Git Bash or your preferred Git client
   - Run: `git clone https://github.com/JohnniesHere/ImGui-JSON-project.git`

3. Create a Branch:
   - Navigate to the project directory: `cd ImGui-JSON-project`
   - Create and switch to a new branch: `git checkout -b [your-branch-name]`
   - Use a descriptive name for your branch, e.g., `feature-user-authentication`

5. Make Changes:
   - Make your changes to the code using Visual Studio

6. Commit Changes:
   - Stage your changes: `git add .`
   - Commit your changes: `git commit -m "Descriptive commit message"`

7. Push Changes:
   - Push your branch to GitHub: `git push origin [your-branch-name]`

8. Create a Pull Request:
   - Go to the repository page on GitHub
   - Click on "Pull requests" > "New pull request"
   - Select your branch and create the pull request
   - Add a description of your changes
   - The project owner will review your changes and merge them if approved

9. Stay Updated:
   - Regularly pull changes from the main branch:
     ```
     git checkout main
     git pull origin main
     git checkout [your-branch-name]
     git merge main
     ```

10. Resolve Conflicts:
    - If there are merge conflicts, resolve them in Visual Studio
    - Commit the resolved changes

Remember to communicate with your team about which files you're working on to avoid merge conflicts.





