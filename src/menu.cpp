#include <Windows.h>
extern "C" __declspec(dllexport) bool BedrockMenuInitialize() noexcept { return true; }
extern "C" __declspec(dllexport) void BedrockMenuShutdown() noexcept {}
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
