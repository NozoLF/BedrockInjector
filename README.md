# Bedrock In-Process DLL Load Test

This repository builds `Injector.exe` and `BedrockMenu.dll` for Windows x64.

1. Open Minecraft Bedrock on Windows.
2. Run Injector.exe, select BedrockMenu.dll, find Minecraft, and click Inject DLL.
3. If DLL loading is permitted, a **separate topmost Win32 test window** appears. INSERT toggles it.

## Limitations

- This is a real remote LoadLibrary attempt, not a Minecraft DirectX/ImGui hook.
- The menu is a separate Win32 window created from the DLL thread. It is **not rendered inside Minecraft's framebuffer** and may be hidden by fullscreen exclusive mode.
- No ESP, X-Ray, Kill Aura, version-specific game data access, or anti-cheat bypass.
- Some Bedrock builds are protected/app-container processes; OpenProcess/CreateRemoteThread can be denied and injection may fail.
- Remote LoadLibrary thread exit status is not a reliable 64-bit module-handle verification; the UI confirms only the load request, not module execution. Verify the visible test window.
- DLL injection can be detected by Windows security software. Do not disable protection to run it.

Build locally: `cmake -S . -B build -A x64 && cmake --build build --config Release`.
GitHub: Actions > Build Injector and Menu DLL > Run workflow > Artifacts.
