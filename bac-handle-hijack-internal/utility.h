#pragma once
#include <windows.h>
#include <ntstatus.h>
#include <iostream>
#include <tlhelp32.h>
#include <ntstatus.h>

#include "undefs.h"

namespace utility
{
	HANDLE find_hijackable_handle(int pid);
	int get_pid_by_name(std::string name);
	void inject_dll_loadlibrary(HANDLE process_handle, LPCSTR dll_path);
}

