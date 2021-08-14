#pragma once
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>

namespace utility
{
	int get_pid_by_name(std::string name);
	void inject_dll_loadlibrary(int pid, LPCSTR dll_path);
}