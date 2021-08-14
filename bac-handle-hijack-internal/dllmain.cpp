#include "utility.h"

BOOLEAN WINAPI main(HMODULE hModule)
{
    FILE *pFile = nullptr;
    AllocConsole();
    freopen_s(&pFile, "CONOUT$", "w", stdout);

    printf_s("[>] bac-handle-hijack injected\n");

    int game_pid = 0;
    while (!(game_pid = utility::get_pid_by_name("javaw.exe")))
    {
        printf_s("[>] waiting for javaw process\n");
        Sleep(2000);
    }

    printf_s("[>] found javaw process -> %d\n", game_pid);

    HANDLE hijacked_handle = nullptr; 
    hijacked_handle = utility::find_hijackable_handle(game_pid);

    if (hijacked_handle != INVALID_HANDLE_VALUE)
    {
        printf_s("[>] found full control handle to javaw -> %p\n", hijacked_handle);

        utility::inject_dll_loadlibrary(hijacked_handle, "C:\\Users\\zpes\\source\\repos\\bac-handle-hijack\\x64\\Release\\bac-handle-hijack-target.dll");

        printf_s("[>] injected dll, cleaning up\n");

        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
    }

    printf_s("[>] failed to find handle, cleaning up\n");
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)main, hModule, 0, 0);
    }
    return TRUE;
}

