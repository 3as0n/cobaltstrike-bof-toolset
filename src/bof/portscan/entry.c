#include "bofdefs.h"
#include "base.c"
#include <winbase.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void portscan(char * target, char * port){
	WSADATA wsaData;
        SOCKET ConnectSocket = INVALID_SOCKET;
        struct addrinfo *result = NULL,
                        *ptr = NULL,
                        hints;

        int iResult;

        // Initialize Winsock
        iResult = Ws2_32$WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            BeaconPrintf(CALLBACK_OUTPUT,"WSAStartup failed with error: %d\n", iResult);

        }

        KERNEL32$RtlZeroMemory( &hints, sizeof(hints) );
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        //
        // Resolve the server address and port
        iResult = Ws2_32$getaddrinfo(target, port, &hints, &result);
        if ( iResult != 0 ) {
            BeaconPrintf(CALLBACK_OUTPUT,"getaddrinfo failed with error: %d\n", iResult);
            Ws2_32$WSACleanup();

        }

        // Attempt to connect to an address until one succeeds
        for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = Ws2_32$socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                BeaconPrintf(CALLBACK_OUTPUT,"socket failed with error: %ld\n", Ws2_32$WSAGetLastError());
                Ws2_32$WSACleanup();

            }

            // Connect to server.
            iResult = Ws2_32$connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                Ws2_32$closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }

        Ws2_32$freeaddrinfo(result);

        if (ConnectSocket == INVALID_SOCKET) {
            BeaconPrintf(CALLBACK_OUTPUT,"Port %s not open on %s\n", port, target);
            Ws2_32$WSACleanup();

        }else {
                BeaconPrintf(CALLBACK_OUTPUT, "Port %s open on %s\n", port, target);
            }

    	// cleanup
    	Ws2_32$closesocket(ConnectSocket);
    	Ws2_32$WSACleanup();
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	//input param
	datap parser;
        char * target;
        char * port;

        BeaconDataParse(&parser, Buffer, Length);
        target = BeaconDataExtract(&parser, NULL);
        port = BeaconDataExtract(&parser, NULL);

	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	portscan(target, port);
	printoutput(TRUE);
};

#else

int main(int argc, char ** argv)
{
	//code for standalone exe for scanbuild / leak checks
	char * target = argv[1];
	char * port = argv[2];
	portscan(target, port);
	return 0;
}

#endif
