#include "main.h"

int __stdcall WinMain(HINSTANCE something, HINSTANCE somethingelse, LPSTR cmd, int cmdshow)
{
	Injector inj;
	//inj.OpenConsole("bible");
	inj.AutoStart();
	inj.Execute("module.dll");
	//while (true)
	//	Sleep(1);
	exit(0);
}