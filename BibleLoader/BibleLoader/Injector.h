#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
class Injector
{
protected:
private:
	HWND hWnd = 0;

	STARTUPINFO*			sInfo = new STARTUPINFO();
	PROCESS_INFORMATION*	pInfo = new PROCESS_INFORMATION();
public:
	void OpenConsole(const char* Title);
	void AutoStart();
	void Execute(std::string name);
}; extern Injector* inj;