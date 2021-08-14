#include "utility.h"

int utility::get_pid_by_name(std::string name)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snap, &entry))
		{
			do
			{
				if (name.compare(entry.szExeFile) == 0)
				{
					return entry.th32ProcessID;
				}
			} 			
			while (Process32Next(snap, &entry));
		}
	}
}

void utility::inject_dll_loadlibrary(int pid, LPCSTR dll_path)
{
	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

	if (process_handle != INVALID_HANDLE_VALUE)
	{
		void* param_alloc = VirtualAllocEx(process_handle, 0, strlen(dll_path) + 1, MEM_COMMIT, PAGE_READWRITE);

		if (param_alloc)
		{
			if (WriteProcessMemory(process_handle, param_alloc, (void*)dll_path, strlen(dll_path) + 1, 0))
			{
				HMODULE kernelbase_h = GetModuleHandle("kernelbase.dll");

				if (!kernelbase_h)
					return;

				uintptr_t LoadLibraryA_t = (uintptr_t)GetProcAddress(kernelbase_h, "LoadLibraryA");

				if (!LoadLibraryA_t)
					return;

				HANDLE thread_handle = CreateRemoteThread(process_handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA_t, param_alloc, 0, 0);

				WaitForSingleObject(thread_handle, INFINITE);
			}
		}
	}
}
