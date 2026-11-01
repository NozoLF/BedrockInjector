#define UNICODE
#define _UNICODE
#define NOMINMAX
#include <Windows.h>
#include <TlHelp32.h>
#include <commdlg.h>
#include <string>
#include <filesystem>

namespace {
HWND g_window{}, g_path{}, g_status{};
std::wstring g_dll;
constexpr int kBrowse=1001, kFind=1002, kLoad=1003;
void status(const wchar_t* s) { SetWindowTextW(g_status,s); }
DWORD findMinecraft() {
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(snapshot==INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W entry{}; entry.dwSize=sizeof(entry);
    DWORD pid=0;
    if(Process32FirstW(snapshot,&entry)) do {
        if(lstrcmpiW(entry.szExeFile,L"Minecraft.Windows.exe")==0) { pid=entry.th32ProcessID; break; }
    } while(Process32NextW(snapshot,&entry));
    CloseHandle(snapshot); return pid;
}
void browse() {
    wchar_t path[32768]{};
    OPENFILENAMEW dlg{}; dlg.lStructSize=sizeof(dlg); dlg.hwndOwner=g_window;
    dlg.lpstrFile=path; dlg.nMaxFile=32768;
    dlg.lpstrFilter=L"DLL files (*.dll)\0*.dll\0All files (*.*)\0*.*\0";
    dlg.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
    if(GetOpenFileNameW(&dlg)) { g_dll=path; SetWindowTextW(g_path,g_dll.c_str()); status(L"DLL selected."); }
}
// This standalone loader only loads explicitly selected DLLs into itself.
void loadLocal() {
    if(g_dll.empty() || !std::filesystem::is_regular_file(g_dll)) { status(L"Select an existing DLL first."); return; }
    HMODULE module=LoadLibraryW(g_dll.c_str());
    if(!module) { status(L"DLL load failed. Check DLL architecture and dependencies."); return; }
    status(L"DLL loaded into Injector.exe (not Minecraft).");
}
LRESULT CALLBACK wndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_COMMAND) {
        switch(LOWORD(wp)) {
            case kBrowse: browse(); return 0;
            case kFind: { DWORD pid=findMinecraft(); std::wstring s=pid ? L"Minecraft found. PID: "+std::to_wstring(pid) : L"Minecraft not running."; SetWindowTextW(g_status,s.c_str()); return 0; }
            case kLoad: loadLocal(); return 0;
        }
    }
    if(msg==WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int show) {
    WNDCLASSW wc{}; wc.hInstance=instance; wc.lpfnWndProc=wndProc; wc.lpszClassName=L"BedrockDllPickerWindow";
    wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    if(!RegisterClassW(&wc)) return 1;
    g_window=CreateWindowExW(0,wc.lpszClassName,L"Bedrock DLL Picker",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,660,220,nullptr,nullptr,instance,nullptr);
    if(!g_window) return 2;
    CreateWindowW(L"STATIC",L"Selected DLL:",WS_CHILD|WS_VISIBLE,18,18,600,20,g_window,nullptr,instance,nullptr);
    g_path=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|ES_READONLY|ES_AUTOHSCROLL,18,44,480,26,g_window,nullptr,instance,nullptr);
    CreateWindowW(L"BUTTON",L"Browse...",WS_CHILD|WS_VISIBLE,508,44,125,26,g_window,(HMENU)kBrowse,instance,nullptr);
    CreateWindowW(L"BUTTON",L"Find Minecraft",WS_CHILD|WS_VISIBLE,18,85,170,32,g_window,(HMENU)kFind,instance,nullptr);
    CreateWindowW(L"BUTTON",L"Load DLL locally",WS_CHILD|WS_VISIBLE,198,85,170,32,g_window,(HMENU)kLoad,instance,nullptr);
    g_status=CreateWindowW(L"STATIC",L"Select a DLL. Minecraft injection is not implemented.",WS_CHILD|WS_VISIBLE,18,135,615,38,g_window,nullptr,instance,nullptr);
    ShowWindow(g_window,show);
    MSG msg{}; while(GetMessageW(&msg,nullptr,0,0)>0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return static_cast<int>(msg.wParam);
}
