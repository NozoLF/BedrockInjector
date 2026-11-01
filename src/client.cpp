#include <Windows.h>

static DWORD WINAPI ShowLoaded(LPVOID) {
    MessageBoxW(nullptr, L"ClientTest.dll loaded into this process.\nThis is a DLL loading test, not an in-game overlay.", L"Client Test", MB_OK | MB_ICONINFORMATION);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        HANDLE thread = CreateThread(nullptr, 0, ShowLoaded, nullptr, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
