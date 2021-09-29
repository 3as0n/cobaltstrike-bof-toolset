
#include "bofdefs.h"
#include "base.c"
#include <dsgetdc.h>


//#pragma comment(lib, "Netapi32.lib")
void DomainInfo(){
	DWORD dwRet;
	PDOMAIN_CONTROLLER_INFO pdcInfo;

	dwRet = NETAPI32$DsGetDcNameA(NULL, NULL, NULL, NULL, 0, &pdcInfo);
	if (ERROR_SUCCESS == dwRet) {
        BeaconPrintf(CALLBACK_OUTPUT, "Domain Forest Name: %s\n"
        "Domain: %s\n"
        "Domain Controller: %s\n"
        "Domain Controller Address: %s\n"
        "DC Site Name: %s\n",
        pdcInfo->DnsForestName,
        pdcInfo->DomainName,
        pdcInfo->DomainControllerName,
        pdcInfo->DomainControllerAddress,
        pdcInfo->DcSiteName);
	}

	NETAPI32$NetApiBufferFree(pdcInfo);
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length)
{
	if(!bofstart())
	{
		return;
	}

	DomainInfo();
	printoutput(TRUE);
};
#else
int main(int argc, char ** argv){
	DomainInfo();
	return 0;
}
#endif
