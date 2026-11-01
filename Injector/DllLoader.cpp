#include "Injector/DllLoader.h"
#include <TlHelp32.h>
#include <filesystem>
#include <memory>
namespace {
struct Handle { HANDLE h{}; ~Handle(){if(h && h!=INVALID_HANDLE_VALUE)CloseHandle(h);} operator HANDLE()const{return h;} };
std::wstring ErrorMessage(DWORD e){wchar_t* msg=nullptr;DWORD n=FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,e,0,reinterpret_cast<LPWSTR>(&msg),0,nullptr);std::wstring s=n?std::wstring(msg,n):L"Unknown error";if(msg)LocalFree(msg);return s;}
bool IsLoaded(DWORD pid,const std::wstring& file){Handle snap{CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid)};if(snap.h==INVALID_HANDLE_VALUE)return false;MODULEENTRY32W mod{};mod.dwSize=sizeof(mod);if(Module32FirstW(snap,&mod))do{if(_wcsicmp(mod.szExePath,file.c_str())==0)return true;}while(Module32NextW(snap,&mod));return false;}
LoadResult Fail(DWORD code,const wchar_t* what){return {false,code,std::wstring(what)+L": "+ErrorMessage(code)};}
}
LoadResult LoadDllIntoProcess(DWORD pid,const std::wstring& path){
 if(!pid)return {false,ERROR_INVALID_PARAMETER,L"Minecraft process not found"};
 std::error_code ec;auto canonical=std::filesystem::canonical(path,ec);if(ec||!std::filesystem::is_regular_file(canonical))return {false,ERROR_FILE_NOT_FOUND,L"DLL path is invalid"};
 const std::wstring dll=canonical.wstring();if(IsLoaded(pid,dll))return {false,ERROR_ALREADY_EXISTS,L"DLL already loaded"};
 Handle process{OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,pid)};
 if(!process.h)return Fail(GetLastError(),L"OpenProcess failed");
 USHORT processMachine=0,nativeMachine=0;
 if(!IsWow64Process2(process,&processMachine,&nativeMachine))return Fail(GetLastError(),L"Architecture check failed");
 if(processMachine!=IMAGE_FILE_MACHINE_UNKNOWN||nativeMachine!=IMAGE_FILE_MACHINE_AMD64)return {false,ERROR_BAD_EXE_FORMAT,L"Target must be native x64"};
 HMODULE kernel=GetModuleHandleW(L"kernel32.dll");auto load=reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel,"LoadLibraryW"));if(!load)return Fail(GetLastError(),L"LoadLibraryW lookup failed");
 SIZE_T bytes=(dll.size()+1)*sizeof(wchar_t);void* remote=VirtualAllocEx(process,nullptr,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!remote)return Fail(GetLastError(),L"VirtualAllocEx failed");
 SIZE_T written=0;if(!WriteProcessMemory(process,remote,dll.c_str(),bytes,&written)||written!=bytes){DWORD e=GetLastError();VirtualFreeEx(process,remote,0,MEM_RELEASE);return Fail(e,L"WriteProcessMemory failed");}
 Handle thread{CreateRemoteThread(process,nullptr,0,load,remote,0,nullptr)};
 if(!thread.h){DWORD e=GetLastError();VirtualFreeEx(process,remote,0,MEM_RELEASE);return Fail(e,L"CreateRemoteThread failed");}
 DWORD wait=WaitForSingleObject(thread,15000);
 if(wait!=WAIT_OBJECT_0){if(wait==WAIT_TIMEOUT)return {false,WAIT_TIMEOUT,L"DLL load timed out; remote memory retained until target exits"};return Fail(GetLastError(),L"WaitForSingleObject failed");}
 DWORD exitCode=0;bool exitOk=GetExitCodeThread(thread,&exitCode)!=0;
 VirtualFreeEx(process,remote,0,MEM_RELEASE);
 if(!exitOk)return Fail(GetLastError(),L"GetExitCodeThread failed");
 if(!exitCode)return {false,ERROR_DLL_INIT_FAILED,L"Remote LoadLibraryW returned null"};
 return {true,0,L"DLL load request completed. Ingame rendering is not yet implemented."};
}
