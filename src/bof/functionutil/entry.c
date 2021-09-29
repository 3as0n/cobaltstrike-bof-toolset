#include "bofdefs.h"
#include "base.c"
#include "winternl.h"

#define SYSCALL_STUB_SIZE 8

BOOL GetLoadedModuleFunctionBytes(char * moduleName, char * functionName, LPVOID loadedStub){
	SIZE_T bRead = 0;

	HMODULE module = NULL;
	FARPROC funcAddress = NULL;

	module = KERNEL32$LoadLibraryA((LPCSTR)moduleName);

	if (module != NULL)
	{
		funcAddress = KERNEL32$GetProcAddress((HMODULE)module, (LPCSTR)functionName);
		if (funcAddress != NULL)
		{
			if (KERNEL32$ReadProcessMemory(KERNEL32$GetCurrentProcess(), funcAddress, loadedStub, SYSCALL_STUB_SIZE, &bRead)) {
				return TRUE;
			} else {
				BeaconPrintf(CALLBACK_ERROR, "ReadProcessMemory failed\n");
			}
		} else {
		BeaconPrintf(CALLBACK_ERROR, "Failed to find function address in loaded module\n");
		}

	} else {
		BeaconPrintf(CALLBACK_ERROR, "Could not load library\n");
	}
	return FALSE;
}


PVOID RVAtoRawOffset(DWORD_PTR RVA, PIMAGE_SECTION_HEADER section)
{
	return (PVOID)(RVA - section->VirtualAddress + section->PointerToRawData);
}


void PatchFunction(char * moduleName, char * functionName, unsigned char * stub) {
	unsigned char buf[8];
	SIZE_T bRead = 0;

	HMODULE module = NULL;
	FARPROC funcAddress = NULL;

	module = KERNEL32$LoadLibraryA((LPCSTR)moduleName);

	if (module != NULL)
	{
		funcAddress = KERNEL32$GetProcAddress((HMODULE)module, (LPCSTR)functionName);
		if (funcAddress != NULL)
		{
			DWORD oldProt;
			KERNEL32$VirtualProtect(funcAddress, 4, PAGE_READWRITE, &oldProt);

			MSVCRT$memcpy(funcAddress, stub, SYSCALL_STUB_SIZE);

			DWORD oldOldProt;
			KERNEL32$VirtualProtect(funcAddress, 4, oldProt, &oldOldProt);

			BeaconPrintf(CALLBACK_OUTPUT, "[+] Patched %s function successfully.", functionName);
		} else {
		BeaconPrintf(CALLBACK_ERROR, "Failed to find function address");
		}
	} else {
		BeaconPrintf(CALLBACK_ERROR, "Could not load %s library", moduleName);
	}
}

void FunctionUtil(char * functionName, char * moduleName, int modeSelect){
	char onDiskPath[100] = "c:\\windows\\system32\\";
	MSVCRT$strcat(onDiskPath, moduleName);

	unsigned char loadedStub[SYSCALL_STUB_SIZE] = {};
	unsigned char ondiskStub[SYSCALL_STUB_SIZE] = {};	
	
	if(modeSelect == 0){   // Read in-memory	
        char *token = NULL;
	    const char s[2] = ","; //delimiter
	    token = MSVCRT$strtok(functionName, s);

        while( token != NULL ) {
			BeaconPrintf(CALLBACK_OUTPUT, "[*] Reading %i bytes of %s function in %s", SYSCALL_STUB_SIZE, token, moduleName);

			if(GetLoadedModuleFunctionBytes(moduleName, token, loadedStub)){
				BeaconPrintf(CALLBACK_OUTPUT, "%x %x %x\n%x %x %x %x %x", loadedStub[0], loadedStub[1], loadedStub[2], loadedStub[3], 
					loadedStub[4], loadedStub[5], loadedStub[6], loadedStub[7]);
			}
            
            token = MSVCRT$strtok(NULL, s);    
        }
	} else {    // Compare or patch from on-disk DLL
	
		if(modeSelect == 1){
			BeaconPrintf(CALLBACK_OUTPUT, "[*] Comparing %i bytes of %s function(s) in %s with on-disk version", SYSCALL_STUB_SIZE, functionName, moduleName);
		} else if (modeSelect == 2) {
			BeaconPrintf(CALLBACK_OUTPUT, "[*] Patching %i bytes of %s function(s) in %s with on-disk version", SYSCALL_STUB_SIZE, functionName, moduleName);
		}

	    char *token = NULL;
	    const char s[2] = ","; //delimiter
	    token = MSVCRT$strtok(functionName, s);

   		HANDLE file = NULL;
		DWORD fileSize;
		DWORD bytesRead;
		LPVOID fileData = NULL;

		file = KERNEL32$CreateFileA((LPCWSTR)onDiskPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		fileSize = KERNEL32$GetFileSize(file, NULL);
		fileData = KERNEL32$HeapAlloc(KERNEL32$GetProcessHeap(), 0, fileSize);
		KERNEL32$ReadFile(file, fileData, fileSize, &bytesRead, NULL);

		PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)fileData;
		PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)fileData + dosHeader->e_lfanew);
		DWORD exportDirRVA = imageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(imageNTHeaders);
		PIMAGE_SECTION_HEADER textSection = section;
		PIMAGE_SECTION_HEADER rdataSection = section;

		for (int i = 0; i < imageNTHeaders->FileHeader.NumberOfSections; i++) 
		{
			if (MSVCRT$strcmp((CHAR*)section->Name, (CHAR*)".rdata") == 0) { 
				rdataSection = section;
				break;
			}
			section++;
		}

		PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)RVAtoRawOffset((DWORD_PTR)fileData + exportDirRVA, rdataSection);

		PDWORD addressOfNames = (PDWORD)RVAtoRawOffset((DWORD_PTR)fileData + *(&exportDirectory->AddressOfNames), rdataSection);
		PDWORD addressOfFunctions = (PDWORD)RVAtoRawOffset((DWORD_PTR)fileData + *(&exportDirectory->AddressOfFunctions), rdataSection);

		while( token != NULL ) {

			if(GetLoadedModuleFunctionBytes(moduleName, token, loadedStub)){	

				for (size_t i = 0; i < exportDirectory->NumberOfNames; i++)
				{
					DWORD_PTR functionNameVA = (DWORD_PTR)RVAtoRawOffset((DWORD_PTR)fileData + addressOfNames[i], rdataSection);
					DWORD_PTR functionVA = (DWORD_PTR)RVAtoRawOffset((DWORD_PTR)fileData + addressOfFunctions[i + 1], textSection);
					LPCSTR functionNameResolved = (LPCSTR)functionNameVA;

					if (MSVCRT$strcmp(functionNameResolved, token) == 0)
					{
						MSVCRT$memcpy(ondiskStub, (LPVOID)functionVA, SYSCALL_STUB_SIZE);
				
						if (modeSelect == 1) {    //Check
							if(MSVCRT$memcmp(loadedStub, ondiskStub, SYSCALL_STUB_SIZE) != 0) {
							BeaconPrintf(CALLBACK_OUTPUT, "[!] %s function doesn't match on-disk.", token);
							BeaconPrintf(CALLBACK_OUTPUT, "Loaded Module: %x %x %x %x %x %x %x %x\nOndisk Module: %x %x %x %x %x %x %x %x", 
								loadedStub[0], loadedStub[1], loadedStub[2], loadedStub[3], 
								loadedStub[4], loadedStub[5], loadedStub[6], loadedStub[7], 
								ondiskStub[0], ondiskStub[1], ondiskStub[2], ondiskStub[3], 
								ondiskStub[4], ondiskStub[5], ondiskStub[6], ondiskStub[7]);
							} else {
								BeaconPrintf(CALLBACK_OUTPUT, "[+] %s function matches on-disk.", token);
							}
						} else if (modeSelect == 2) { //Patch
							PatchFunction(moduleName, functionName, ondiskStub);
						}
						break;	
					}
				}
			} 
		    token = MSVCRT$strtok(NULL, s); 
		}
		NTDLL$NtClose(file);
	}
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	//input param
	datap parser;	
	BeaconDataParse(&parser, Buffer, Length);
	
	char * functionName = NULL;
	char * moduleName = NULL;

	int modeSelect = 0; 

	moduleName = BeaconDataExtract(&parser, NULL);
	functionName = BeaconDataExtract(&parser, NULL);
	modeSelect = BeaconDataInt(&parser);

	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	FunctionUtil(functionName, moduleName, modeSelect);
	printoutput(TRUE);
};

#else

int main(int argc, char ** argv)
{
	
	char * functionName = argv[2];
	char * moduleName = argv[1];
	int modeSelect = argv[3]; 

	//code for standalone exe for scanbuild / leak checks
	FunctionUtil(functionName, moduleName, modeSelect);
	return 0;
}

#endif
