# Bedrock Injector — DLL Loading Test

Windows x64 C++20 demonstration of process discovery and DLL loading into an authorized target process.

## Build
GitHub Actions > Build Windows Injector > Run workflow. Download `Injector-Windows-x64` artifact and extract both files into the same folder.

## Use
Launch an authorized x64 test process, enter its executable name, click Find process, then Load ClientTest.dll. A successful DLL load shows a Windows message box in the target process.

## Limitations
This is a **generic DLL loading test**, not a functioning Minecraft Bedrock cheat client. It does not implement an ImGui in-game menu, ESP, X-Ray, Kill Aura, or bypasses. Minecraft may block injection or behave differently depending on platform protections and version. The local `LoadLibraryW` address is not guaranteed to be valid in every remote process. No universal compatibility is claimed.
