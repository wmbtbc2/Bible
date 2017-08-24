#include "Injector.h"
Injector* inj;

void Injector::OpenConsole(const char* Title) {
	//Alocate a console used for output
	FILE* fp;
	AllocConsole();
	SetConsoleTitleA(Title);
	freopen_s(&fp, "CONOUT$", "w", stdout);
}

void Injector::AutoStart() {
	//find csgo
	hWnd = FindWindow(0, L"Counter-Strike: Global Offensive"); 

	//if exists, return,
	if (hWnd) 
		return;

	//start csgo by path, might need to change depending on where you installed
	if (!CreateProcess(L"C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo.exe", L"-secure -steam -novid", nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, sInfo, pInfo))
	{
		MessageBoxA(0, "Error 1", "CreateProcess Fail", MB_OK);
		exit(0);
	}

	//wait until csgo has been found
	while (!(hWnd = FindWindow(0, L"Counter-Strike: Global Offensive")))
		Sleep(1);

	//give time for csgo to its own load modules
	Sleep(5000);
}

void Injector::Execute(std::string name) {

	// retrieve process id by handle using GetWindowThreadProcessId
	DWORD pId;
	GetWindowThreadProcessId(hWnd, &pId);
	if (!pId) {
		MessageBoxA(0, "Error 2", "GetWindowThreadProcessId Fail", MB_OK);
		exit(0);
	}

	//open a handle to the process with hightened rights
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
	if (!hProcess) {
		MessageBoxA(0, "Error 3", "OpenProcess Fail", MB_OK);
		exit(0);
	}

	//load the address of the libraries function we need to use in order to inject
	LPVOID LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (!LoadLibAddy) {
		MessageBoxA(0, "Error 4", "GetProcAddress Fail", MB_OK);
		exit(0);
	}

	//retrieve path of our dll
	char Dll[32];
	GetFullPathNameA(name.c_str(), MAX_PATH, Dll, NULL);
	if (!Dll) {
		MessageBoxA(0, "Error 5", "GetFullPathNameA Fail", MB_OK);
		exit(0);
	}

	//allocate a space in memory for our dll
	LPVOID AllocMem = VirtualAllocEx(hProcess, NULL, strlen(Dll), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!AllocMem)
	{
		MessageBoxA(0, "Error 6", "VirtualAllocEx Fail", MB_OK);
		exit(0);
	}

	//write our dll into memory
	if (!WriteProcessMemory(hProcess, AllocMem, Dll, strlen(Dll), NULL))
	{
		MessageBoxA(0, "Error 6", "WriteProcessMemory Fail", MB_OK);
		exit(0);
	}

	//create a thread in virtual space to execute our dll
	HANDLE ThreadHandle = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, AllocMem, 0, NULL);
	if (!ThreadHandle)
	{
		MessageBoxA(0, "Error 7", "CreateRemoteThread Fail", MB_OK);
		exit(0);
	}

	//wait until the thread has completed
	//what it needs to do before we continue
	WaitForSingleObject(ThreadHandle, INFINITE);

	//free virtual memory
	VirtualFreeEx(hProcess, AllocMem, strlen(Dll), MEM_RELEASE);

	//close handle
	CloseHandle(ThreadHandle);

	//close handle
	CloseHandle(hProcess);
}