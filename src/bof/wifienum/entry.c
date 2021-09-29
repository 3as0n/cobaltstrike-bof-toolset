
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

void wifienum(){
	HANDLE hClient = NULL;
        DWORD dwMaxClient = 2;      //    
        DWORD dwCurVersion = 0;
        DWORD dwResult = 0;
        DWORD dwRetVal = 0;
        int iRet = 0;      
        WCHAR GuidString[39] = {0};
        unsigned int i, j;

        PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
        PWLAN_INTERFACE_INFO pIfInfo = NULL;

        PWLAN_PROFILE_INFO_LIST pProfileList = NULL;
        PWLAN_PROFILE_INFO pProfile = NULL;

        dwResult = WLANAPI$WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
        if (dwResult != ERROR_SUCCESS) {
            BeaconPrintf(CALLBACK_OUTPUT, "WlanOpenHandle failed with error: %u\n", dwResult);
        }

        dwResult = WLANAPI$WlanEnumInterfaces(hClient, NULL, &pIfList);
        if (dwResult != ERROR_SUCCESS) {
            BeaconPrintf(CALLBACK_OUTPUT, "WlanEnumInterfaces failed with error: %u\n", dwResult);
        } else {
   
            for (i = 0; i < (int) pIfList->dwNumberOfItems; i++) {
                pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i];
                dwResult = WLANAPI$WlanGetProfileList(hClient,
                                                &pIfInfo->InterfaceGuid,
                                                NULL, 
                                                &pProfileList);
                if (dwResult != ERROR_SUCCESS) {
                    BeaconPrintf(CALLBACK_OUTPUT, "WlanGetProfileList failed with error: %u\n",
                            dwResult);
                    dwRetVal = 1;
                } else {

                    for (j = 0; j < pProfileList->dwNumberOfItems; j++) {
                        pProfile =
                            (WLAN_PROFILE_INFO *) & pProfileList->ProfileInfo[j];
                        BeaconPrintf(CALLBACK_OUTPUT, "  Profile Name[%u]:  %ws\n", j, pProfile->strProfileName);
                        
                    }
                }
            }
        }
        if (pProfileList != NULL) {
            WLANAPI$WlanFreeMemory(pProfileList);
            pProfileList = NULL;
        }

        if (pIfList != NULL) {
            WLANAPI$WlanFreeMemory(pIfList);
            pIfList = NULL;
        }   
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	wifienum();
	printoutput(TRUE);
};

#else

int main()
{
	//code for standalone exe for scanbuild / leak checks
	wifienum();
	return 0;
}

#endif
