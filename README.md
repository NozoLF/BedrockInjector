# Bedrock DLL Picker

Windows x64 standalone application with DLL file chooser, Minecraft process detection and local DLL load test.

**It does not inject DLLs into Minecraft or provide an in-game menu.** Loading a DLL into the picker process is not the same as loading it into Minecraft.

## GitHub build

Upload all project files including `.github/workflows/windows.yml` to GitHub. Open Actions > Build Windows EXE > Run workflow, then download the artifact.

## Local build

`cmake -S . -B build -A x64`

`cmake --build build --config Release`

Only load DLLs you trust.
