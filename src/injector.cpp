#include <Windows.h>
#include <TlHelp32.h>
#include <commdlg.h>
#include <string>
#include <vector>

constexpr wchar_t kTarget[] = L"Minecraft.Windows.exe";
HWND g_status{};
std::wstring g_path;
void Status(const std::wstring& value) { SetWindowTextW(g_status, value.c_str()); }
DWORD FindMinecraft() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W entry{sizeof(entry)};
    DWORD pid = 0;
    if (Process32FirstW(snapshot, &entry)) do {
        if (_wcsicmp(entry.szExeFile, kTarget) == 0) { pid = entry.th32ProcessID; break; }
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
    return pid;
}
std::wstring ErrorText(DWORD code) { return L"Win32 error: " + std::to_wstring(code); }
bool Inject(DWORD pid, const std::wstring& path, std::wstring& error) {
    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    if (!process) { error = L"OpenProcess: " + ErrorText(GetLastError()); return false; }
    const SIZE_T bytes = (path.size() + 1) * sizeof(wchar_t);
    void* remote = VirtualAllocEx(process, nullptr, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!remote) { error = L"VirtualAllocEx: " + ErrorText(GetLastError()); CloseHandle(process); return false; }
    bool success = false;
    if (!WriteProcessMemory(process, remote, path.c_str(), bytes, nullptr)) {
        error = L"WriteProcessMemory: " + ErrorText(GetLastError());
    } else {
        HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
        auto load = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel, "LoadLibraryW"));
        HANDLE thread = load ? CreateRemoteThread(process, nullptr, 0, load, remote, 0, nullptr) : nullptr;
        if (!thread) error = L"CreateRemoteThread: " + ErrorText(GetLastError());
        else {
            DWORD wait = WaitForSingleObject(thread, 10000);
            DWORD exitCode = 0;
            if (wait == WAIT_OBJECT_0 && GetExitCodeThread(thread, &exitCode) && exitCode != 0) success = true;
            else error = L"LoadLibrary failed or timed out; verify target permissions and DLL dependencies.";
            CloseHandle(thread);
        }
    }
    // Do not free the remote path while a timed-out remote thread might still read it.
    if (success || error.find(L"timed out") == std::wstring::npos) VirtualFreeEx(process, remote, 0, MEM_RELEASE);
    CloseHandle(process);
    return success;
}
LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_COMMAND) {
        switch (LOWORD(wp)) {
        case 1: {
            wchar_t file[MAX_PATH]{};
            OPENFILENAMEW dialog{sizeof(dialog)};
            dialog.hwndOwner = hwnd; dialog.lpstrFilter = L"DLL Files\0*.dll\0\0";
            dialog.lpstrFile = file; dialog.nMaxFile = MAX_PATH;
            dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            if (GetOpenFileNameW(&dialog)) { g_path = file; Status(L"Selected: " + g_path); }
            break;
        }
        case 2: {
            DWORD pid = FindMinecraft();
            Status(pid ? L"Minecraft found, PID " + std::to_wstring(pid) : L"Minecraft.Windows.exe not found");
            break;
        }
        case 3: {
            DWORD pid = FindMinecraft();
            if (!pid) { Status(L"Start Minecraft Bedrock first."); break; }
            if (g_path.empty()) { Status(L"Select BedrockMenu.dll first."); break; }
            std::wstring error;
            Status(Inject(pid, g_path, error) ? L"DLL load request completed. Press INSERT in Minecraft." : error);
            break;
        }
        }
        return 0;
    }
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    WNDCLASSW wc{}; wc.lpfnWndProc = Proc; wc.hInstance = instance;
    wc.lpszClassName = L"BedrockInjectorWindow"; wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) return 1;
    HWND window = CreateWindowExW(0, wc.lpszClassName, L"Bedrock DLL Injector - Windows x64", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 230, nullptr, nullptr, instance, nullptr);
    if (!window) return 2;
    CreateWindowW(L"BUTTON", L"Choose DLL", WS_CHILD | WS_VISIBLE, 16, 20, 160, 36, window, (HMENU)1, instance, nullptr);
    CreateWindowW(L"BUTTON", L"Find Minecraft", WS_CHILD | WS_VISIBLE, 190, 20, 160, 36, window, (HMENU)2, instance, nullptr);
    CreateWindowW(L"BUTTON", L"Inject DLL", WS_CHILD | WS_VISIBLE, 364, 20, 160, 36, window, (HMENU)3, instance, nullptr);
    g_status = CreateWindowW(L"STATIC", L"Select a DLL. Minecraft must already be running.", WS_CHILD | WS_VISIBLE | SS_LEFT,
        16, 85, 600, 90, window, nullptr, instance, nullptr);
    ShowWindow(window, show);
    MSG msg{}; while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return static_cast<int>(msg.wParam);
}
