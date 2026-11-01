#include <Windows.h>
#include "BedrockBridge/Bridge.h"
extern "C" __declspec(dllexport) int BedrockMenuApiVersion() noexcept {return 1;}
extern "C" __declspec(dllexport) bool BedrockMenuCanAttach() noexcept {bedrock::Bridge bridge;return bridge.attach();}
BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {if(reason==DLL_PROCESS_ATTACH){}return TRUE;}
