#include "Injector/ProcessManager.h"
#include <TlHelp32.h>
#include <vector>
#include <memory>
#include <winver.h>
ProcessInfo FindBedrock() {
 ProcessInfo info{};
 HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
 if(snap==INVALID_HANDLE_VALUE)return info;
 PROCESSENTRY32W pe{}; pe.dwSize=sizeof(pe);
 if(Process32FirstW(snap,&pe))do {
  if(_wcsicmp(pe.szExeFile,L"Minecraft.Windows.exe")!=0)continue;
  info.pid=pe.th32ProcessID;
  HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,info.pid);
  if(process){wchar_t path[32768]{}; DWORD n=32768;
   if(QueryFullProcessImageNameW(process,0,path,&n)){
    info.executable=path;
    DWORD unused=0;DWORD size=GetFileVersionInfoSizeW(path,&unused);
    if(size){std::vector<BYTE> bytes(size);
     if(GetFileVersionInfoW(path,0,size,bytes.data())){
      VS_FIXEDFILEINFO* vi=nullptr;UINT len=0;
      if(VerQueryValueW(bytes.data(),L"\\",reinterpret_cast<void**>(&vi),&len)&&vi){
       info.version=std::to_wstring(HIWORD(vi->dwFileVersionMS))+L"."+std::to_wstring(LOWORD(vi->dwFileVersionMS))+L"."+std::to_wstring(HIWORD(vi->dwFileVersionLS))+L"."+std::to_wstring(LOWORD(vi->dwFileVersionLS));
      }
     }
    }
   }CloseHandle(process);
  }
  break;
 }while(Process32NextW(snap,&pe));
 CloseHandle(snap);return info;
}
