# BedrockDX12Client – source project

## Status

**Not a functional Minecraft in-game client.** The project builds `Injector.exe` and `BedrockMenu.dll`, but intentionally does not claim a working Minecraft DLL injection, DX12 render hook, ImGui UI, ESP, or X-Ray.

The injector detects `Minecraft.Windows.exe`, reads its executable version where available, and lets the user select a DLL. It does **not** inject it. The DLL exports `BedrockMenuApiVersion` and `BedrockMenuCanAttach` for integration diagnostics. The latter returns false until a verified, compatible Minecraft integration is available. ESP provides a standalone world-to-screen projection algorithm for real camera matrices supplied by a future authorized data provider; X-Ray provides a real block identifier filter. Neither receives live game data.

## Build

GitHub Actions: Actions > Windows x64 > Run workflow. Download the ZIP under Artifacts.

Local Windows: `cmake -S . -B build -A x64` then `cmake --build build --config Release`.

## Why integration is missing

No verified in-process DX12 swapchain / command queue ownership or stable game-data API for the current Minecraft Bedrock version was available. Hard-coded offsets, guessed hooks and invented entity data are not substitutes for a working implementation. Do not disable antivirus protection to run unknown injectors.
