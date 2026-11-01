# Bedrock DX12 Client – project scaffold

This repository is a **buildable Windows project scaffold**, not a Minecraft cheat client.

## Build

Run GitHub Actions > Build Windows x64 > Run workflow. Download the ZIP from Artifacts.

## Implemented

- C++20 / CMake x64 EXE and DLL targets
- Windows DLL file picker
- Exported DLL initialization and shutdown entry points
- GitHub Actions build and ZIP packaging

## Not implemented

- Injection into Minecraft.Windows.exe
- DX12 in-process rendering or Dear ImGui
- Insert-key toggle and game input handling
- Minecraft block/entity data access, X-Ray, ESP, combat modules
- Server bypass or game version support

The generated DLL cannot show a Minecraft in-game GUI. No live Windows build or game test was performed here.
