#include <Windows.h>
#include <commdlg.h>
#include <filesystem>
#include <string>
#include "Injector/ProcessManager.h"
namespace {
HWND status{},pathBox{};std::wstring selected;
void Status(const std::wstring& s){SetWindowTextW(status,s.c_str());}
void SelectDll(HWND owner){wchar_t path[32768]{};if(!selected.empty())wcsncpy_s(path,selected.c_str(),_TRUNCATE);
 OPENFILENAMEW f{};f.lStructSize=sizeof(f);f.hwndOwner=owner;f.lpstrFile=path;f.nMaxFile=32768;f.lpstrFilter=L"DLL files\0*.dll\0\0";f.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
 if(GetOpenFileNameW(&f)){selected=path;SetWindowTextW(pathBox,selected.c_str());}}
LRESULT CALLBACK Proc(HWND h,UINT m,WPARAM w,LPARAM l){
 if(m==WM_COMMAND){switch(LOWORD(w)){
 case 101:{auto p=FindBedrock();Status(p.pid?L"Minecraft PID "+std::to_wstring(p.pid)+L"  Version: "+(p.version.empty()?L"unknown":p.version):L"Minecraft.Windows.exe not found");return 0;}
 case 102:SelectDll(h);return 0;
 case 103:{if(selected.empty()||!std::filesystem::exists(selected)){Status(L"Select an existing DLL");return 0;}
  // Loading foreign DLLs into the Minecraft process is not implemented.
  Status(L"DLL selected. Minecraft in-process integration unavailable.");return 0;}
 }}
 if(m==WM_DESTROY){PostQuitMessage(0);return 0;}
 return DefWindowProcW(h,m,w,l);
}
}
int WINAPI wWinMain(HINSTANCE i,HINSTANCE,PWSTR,int show){
 WNDCLASSW c{};c.hInstance=i;c.lpfnWndProc=Proc;c.lpszClassName=L"BedrockDX12ClientSetup";c.hCursor=LoadCursorW(nullptr,IDC_ARROW);if(!RegisterClassW(&c))return 1;
 HWND h=CreateWindowExW(0,c.lpszClassName,L"Bedrock DX12 Client - Integration Status",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,730,280,nullptr,nullptr,i,nullptr);if(!h)return 2;
 CreateWindowW(L"BUTTON",L"Find Minecraft",WS_CHILD|WS_VISIBLE,18,18,170,36,h,(HMENU)101,i,nullptr);
 CreateWindowW(L"BUTTON",L"Select DLL",WS_CHILD|WS_VISIBLE,202,18,150,36,h,(HMENU)102,i,nullptr);
 CreateWindowW(L"BUTTON",L"Check integration",WS_CHILD|WS_VISIBLE,368,18,180,36,h,(HMENU)103,i,nullptr);
 pathBox=CreateWindowW(L"EDIT",L"",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY,18,70,670,28,h,nullptr,i,nullptr);
 status=CreateWindowW(L"STATIC",L"Ready",WS_CHILD|WS_VISIBLE,18,114,680,65,h,nullptr,i,nullptr);
 wchar_t exe[32768]{};GetModuleFileNameW(nullptr,exe,32768);selected=(std::filesystem::path(exe).parent_path()/L"BedrockMenu.dll").wstring();SetWindowTextW(pathBox,selected.c_str());
 ShowWindow(h,show);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return int(msg.wParam);
}
