#include <Windows.h>
#include <commdlg.h>
#include <string>
namespace {
std::wstring selected;
LRESULT CALLBACK Proc(HWND h, UINT m, WPARAM w, LPARAM l) {
 if (m == WM_COMMAND && LOWORD(w) == 1) {
  wchar_t file[MAX_PATH] = {};
  OPENFILENAMEW dialog{}; dialog.lStructSize = sizeof(dialog);
  dialog.hwndOwner = h; dialog.lpstrFile = file; dialog.nMaxFile = MAX_PATH;
  dialog.lpstrFilter = L"DLL Files\0*.dll\0All Files\0*.*\0";
  dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
  if (GetOpenFileNameW(&dialog)) { selected = file; SetWindowTextW(h, (L"DLL selected: " + selected).c_str()); }
  return 0;
 }
 if (m == WM_DESTROY) { PostQuitMessage(0); return 0; }
 return DefWindowProcW(h,m,w,l);
}
}
int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR, int show) {
 WNDCLASSW wc{}; wc.lpfnWndProc=Proc; wc.hInstance=inst; wc.lpszClassName=L"BedrockDX12Setup";
 wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
 if (!RegisterClassW(&wc)) return 1;
 HWND h=CreateWindowExW(0,wc.lpszClassName,L"Bedrock DX12 - DLL Project Setup",WS_OVERLAPPEDWINDOW,
 CW_USEDEFAULT,CW_USEDEFAULT,640,220,nullptr,nullptr,inst,nullptr);
 if (!h) return 2;
 CreateWindowExW(0,L"BUTTON",L"Select DLL",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,20,30,160,35,h,
 reinterpret_cast<HMENU>(static_cast<INT_PTR>(1)),inst,nullptr);
 CreateWindowExW(0,L"STATIC",L"Minecraft injection and DX12 hooking are not implemented.",
 WS_CHILD|WS_VISIBLE,20,90,560,30,h,nullptr,inst,nullptr);
 ShowWindow(h,show);
 MSG msg{}; while (GetMessageW(&msg,nullptr,0,0)>0) {TranslateMessage(&msg); DispatchMessageW(&msg);}
 return static_cast<int>(msg.wParam);
}
