# BedrockDX12Client – source project

## Status

**Not a functional Minecraft in-game client.** The project builds `Injector.exe` and `BedrockMenu.dll`, but intentionally does not claim a working Minecraft DLL injection, DX12 render hook, ImGui UI, ESP, or X-Ray.

The injector detects `Minecraft.Windows.exe`, reads its executable version where available, and lets the user select a DLL. It does **not** inject it. The DLL exports `BedrockMenuApiVersion` and `BedrockMenuCanAttach` for integration diagnostics. The latter returns false until a verified, compatible Minecraft integration is available. ESP provides a standalone world-to-screen projection algorithm for real camera matrices supplied by a future authorized data provider; X-Ray provides a real block identifier filter. Neither receives live game data.

## Build

GitHub Actions: Actions > Windows x64 > Run workflow. Download the ZIP under Artifacts.

Local Windows: `cmake -S . -B build -A x64` then `cmake --build build --config Release`.

## Why integration is missing

No verified in-process DX12 swapchain / command queue ownership or stable game-data API for the current Minecraft Bedrock version was available. Hard-coded offsets, guessed hooks and invented entity data are not substitutes for a working implementation. Do not disable antivirus protection to run unknown injectors.

## Experimental DLL loading (revision 2026-11-01)

The Injector now attempts to load a DLL into the selected Minecraft.Windows.exe process using Windows process APIs. This is an experimental loader, **not** a working Minecraft in-game client. Minecraft/UWP process protection may reject the request. Run only against a process you own and are authorized to modify.

The existing `BedrockMenu.dll` still contains no DX12 present hook, ImGui renderer, game-data integration, X-Ray renderer or ESP entity reader. `BedrockMenuCanAttach()` will continue to return false. The loader reports only the outcome of `LoadLibraryW`; it does not verify menu functionality. The current source has not been compiled or tested against Minecraft in this environment.

Build on Windows with Visual Studio 2022, Windows SDK, CMake, and x64 configuration. Run GitHub Actions `Windows x64` workflow to obtain the binaries.
