??? note "Create Pico SDK Project"

    1. In VSCode, run `>Raspberry Pi Pico: New Pico Project` in the command palette.
    2. In the dialog below, select `C/C++`.
       ![new-project-dialog](images/new-project-dialog.png){ width=70% }

    3. Create a project with the following settings:
         - **Name** ... Enter the project name.
         - **Board type** ... Select your board type.
         - **Location** ... Select the parent directory where the project directory will be created.
         - **Stdio support** ... Leave `Console over USB` **unchecked** when you use LABOPlatform or other USB features because they conflict with each other. You can enable it by editing the `CMakeLists.txt` file later.
         - **Code generation options** ... Check `Generate C++ code`.

    ![new-project](images/new-project.png)

??? note "Open Existing Pico SDK Project"

    1. Open the project folder in VSCode using one of the following methods:
        - In a command prompt, change the current directory to the project folder and run `code .`.
        - In a Explorer, choose the project folder, push `Alt+D` to focus the address bar, and execute `code .`.
        
          ![vscode-from-explorer](images/vscode-from-explorer.png)
    