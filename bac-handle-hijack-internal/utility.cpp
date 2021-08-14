#include "utility.h"

typedef NTSTATUS(NTAPI* NtQuerySystemInformation_t)(ULONG, PVOID, ULONG, PULONG);

HANDLE utility::find_hijackable_handle(int pid)
{
	NtQuerySystemInformation_t NtQuerySystemInformation = reinterpret_cast<NtQuerySystemInformation_t>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation"));
	NTSTATUS status;
	ULONG handleInfoSize = 0x10000;
	
	PSYSTEM_HANDLE_INFORMATION handleInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(malloc(handleInfoSize));

	while ((status = NtQuerySystemInformation(16, handleInfo, handleInfoSize, nullptr)) == STATUS_INFO_LENGTH_MISMATCH)
		handleInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(realloc(handleInfo, handleInfoSize *= 2));

    if (!(status == STATUS_SUCCESS))
        return nullptr;

	for (int i = 0; i < handleInfo->HandleCount; i++)
	{
		SYSTEM_HANDLE handle = handleInfo->Handles[i];

		HANDLE process = reinterpret_cast<HANDLE>(handle.Handle);
		if (handle.ProcessId == GetCurrentProcessId() && GetProcessId(process) == pid)
			return process;
	}

	free(handleInfo);
	return nullptr;
}

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

void utility::inject_dll_loadlibrary(HANDLE process_handle, LPCSTR dll_path)
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
