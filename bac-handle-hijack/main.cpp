#include "utility.h"

BOOLEAN WINAPI main()
{
	int launcher_pid = 0;
	launcher_pid = utility::get_pid_by_name("Badlion Client.exe");

	utility::inject_dll_loadlibrary(launcher_pid, "C:\\Users\\zpes\\source\\repos\\bac-handle-hijack\\x64\\Release\\bac-handle-hijack-internal.dll");
}