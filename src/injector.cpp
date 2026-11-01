#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <filesystem>
#include <cwchar>

constexpr int kProcess = 101, kInject = 102, kRefresh = 103;
static HWND processEdit = nullptr, statusLabel = nullptr;

static void Status(const std::wstring& message) { SetWindowTextW(statusLabel, message.c_str()); }

static DWORD FindProcess(const std::wstring& name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    DWORD pid = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do { if (_wcsicmp(entry.szExeFile, name.c_str()) == 0) { pid = entry.th32ProcessID; break; } }
        while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return pid;
}

static std::wstring LoadClient(DWORD pid, const std::wstring& path) {
    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    if (!process) return L"OpenProcess failed: " + std::to_wstring(GetLastError());
    const SIZE_T bytes = (path.size() + 1) * sizeof(wchar_t);
    LPVOID remote = VirtualAllocEx(process, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote) { DWORD e = GetLastError(); CloseHandle(process); return L"VirtualAllocEx failed: " + std::to_wstring(e); }
    SIZE_T written = 0;
    bool ok = WriteProcessMemory(process, remote, path.c_str(), bytes, &written) && written == bytes;
    if (!ok) { DWORD e = GetLastError(); VirtualFreeEx(process, remote, 0, MEM_RELEASE); CloseHandle(process); return L"WriteProcessMemory failed: " + std::to_wstring(e); }
    // LoadLibraryW is resolved locally. This is a simple same-architecture test loader, not a universal injection mechanism.
    HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
    auto load = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel, "LoadLibraryW"));
    HANDLE thread = load ? CreateRemoteThread(process, nullptr, 0, load, remote, 0, nullptr) : nullptr;
    if (!thread) { DWORD e = GetLastError(); VirtualFreeEx(process, remote, 0, MEM_RELEASE); CloseHandle(process); return L"CreateRemoteThread failed: " + std::to_wstring(e); }
    DWORD wait = WaitForSingleObject(thread, 10000);
    DWORD exitCode = 0;
    if (wait == WAIT_OBJECT_0) GetExitCodeThread(thread, &exitCode);
    CloseHandle(thread);
    if (wait == WAIT_OBJECT_0) VirtualFreeEx(process, remote, 0, MEM_RELEASE);
    CloseHandle(process);
    if (wait != WAIT_OBJECT_0) return L"Load timed out; remote allocation retained.";
    return exitCode ? L"DLL load requested successfully (check target window)." : L"LoadLibraryW failed in target.";
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindowW(L"STATIC", L"Target process name:", WS_CHILD | WS_VISIBLE, 20, 22, 320, 22, hwnd, nullptr, nullptr, nullptr);
            processEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"Minecraft.Windows.exe", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 20, 50, 420, 28, hwnd, reinterpret_cast<HMENU>(kProcess), nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Find process", WS_CHILD | WS_VISIBLE, 20, 95, 180, 34, hwnd, reinterpret_cast<HMENU>(kRefresh), nullptr, nullptr);
            CreateWindowW(L"BUTTON", L"Load ClientTest.dll", WS_CHILD | WS_VISIBLE, 215, 95, 225, 34, hwnd, reinterpret_cast<HMENU>(kInject), nullptr, nullptr);
            statusLabel = CreateWindowW(L"STATIC", L"Ready. Use only on processes you own or are authorized to modify.", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 153, 445, 80, hwnd, nullptr, nullptr, nullptr);
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wp) == kRefresh || LOWORD(wp) == kInject) {
                wchar_t buffer[MAX_PATH]{};
                GetWindowTextW(processEdit, buffer, MAX_PATH);
                DWORD pid = FindProcess(buffer);
                if (!pid) { Status(L"Target process not found."); return 0; }
                if (LOWORD(wp) == kRefresh) { Status(L"Found process PID: " + std::to_wstring(pid)); return 0; }
                wchar_t exePath[MAX_PATH]{};
                GetModuleFileNameW(nullptr, exePath, MAX_PATH);
                auto dll = (std::filesystem::path(exePath).parent_path() / L"ClientTest.dll").wstring();
                if (!std::filesystem::exists(dll)) { Status(L"ClientTest.dll missing next to Injector.exe"); return 0; }
                Status(LoadClient(pid, dll));
            }
            return 0;
        }
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"BedrockInjectorWindow";
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&wc)) return 1;
    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"Bedrock Injector - DLL Loading Test", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 485, 280, nullptr, nullptr, instance, nullptr);
    if (!hwnd) return 2;
    ShowWindow(hwnd, show);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return static_cast<int>(msg.wParam);
}
