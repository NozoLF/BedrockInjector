#include <Windows.h>
#include <atomic>

HMODULE g_module{};
std::atomic_bool g_running{true};
LRESULT CALLBACK MenuProc(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CLOSE) { ShowWindow(window, SW_HIDE); return 0; }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(window, &ps);
        RECT rect{}; GetClientRect(window, &rect);
        FillRect(dc, &rect, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        SetBkMode(dc, TRANSPARENT); SetTextColor(dc, RGB(130, 220, 255));
        const wchar_t* title = L"BEDROCK MENU - DLL TEST";
        TextOutW(dc, 24, 24, title, lstrlenW(title));
        SetTextColor(dc, RGB(235, 235, 235));
        const wchar_t* lines[] = {
            L"INSERT: toggle menu", L"This UI runs in the target process.",
            L"ESP: not implemented", L"X-Ray: not implemented", L"Kill Aura: not implemented",
            L"No game hooks or anti-cheat bypasses are installed."
        };
        for (int i = 0; i < 6; ++i) TextOutW(dc, 24, 68 + i * 29, lines[i], lstrlenW(lines[i]));
        EndPaint(window, &ps); return 0;
    }
    return DefWindowProcW(window, msg, wp, lp);
}
DWORD WINAPI MenuThread(void*) {
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(g_module);
    WNDCLASSW wc{}; wc.lpfnWndProc = MenuProc; wc.hInstance = instance;
    wc.lpszClassName = L"BedrockInjectedMenuTest"; wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);
    HWND window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, wc.lpszClassName,
        L"Bedrock DLL Menu Test", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        140, 140, 500, 330, nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    ShowWindow(window, SW_SHOW);
    bool wasDown = false;
    MSG msg{};
    while (g_running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
        bool down = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
        if (down && !wasDown) ShowWindow(window, IsWindowVisible(window) ? SW_HIDE : SW_SHOW);
        wasDown = down;
        Sleep(20);
    }
    DestroyWindow(window);
    return 0;
}
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_module = instance;
        DisableThreadLibraryCalls(instance);
        HANDLE thread = CreateThread(nullptr, 0, MenuThread, nullptr, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
