#pragma once
#include <Windows.h>
#include <string>
struct LoadResult { bool ok{}; DWORD error{}; std::wstring message; };
LoadResult LoadDllIntoProcess(DWORD pid, const std::wstring& path);
