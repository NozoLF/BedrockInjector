#pragma once
#include <Windows.h>
#include <string>
struct ProcessInfo { DWORD pid{}; std::wstring executable; std::wstring version; };
ProcessInfo FindBedrock();
