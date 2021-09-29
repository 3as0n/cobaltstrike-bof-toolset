#include "bofdefs.h"
#include "base.c"
#include <tchar.h>
#include <wininet.h> 

void sendHttpRequest(char * method, char *host, char *uri, int port, char * useragent, char * headers, char * body, int tls, int isoutput, int proxy){

	DWORD dwAccessType;
	if(proxy==1){
		dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
	} else {
		dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
	}

	HINTERNET hIntSession = WININET$InternetOpenA(useragent, dwAccessType, NULL, NULL, 0);
    HINTERNET hHttpSession = WININET$InternetConnectA(hIntSession, host, port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);

    DWORD internetFlags;
    if (tls == 1){
	    internetFlags = INTERNET_FLAG_RELOAD |INTERNET_FLAG_SECURE;
	} else {
		internetFlags = INTERNET_FLAG_RELOAD;
	}

	HINTERNET hHttpRequest = WININET$HttpOpenRequestA(
      hHttpSession,
      method,
      uri,
      0, 0, 0, internetFlags, 0);

    if( !WININET$HttpSendRequestA(hHttpRequest, headers, KERNEL32$lstrlenA(headers), body, KERNEL32$lstrlenA(body))) {
      BeaconPrintf(CALLBACK_ERROR, "No response.");
    }
    else {
	    int statusCode;
		char responseText[256];
		DWORD responseTextSize = sizeof(responseText);

		//Check existance of page (for 404 error)
		if(!WININET$HttpQueryInfoA(hHttpRequest, HTTP_QUERY_STATUS_CODE, &responseText, &responseTextSize, NULL)){
			BeaconPrintf(CALLBACK_ERROR, "Retrieving HTTP Request info failed");
		}
		BeaconPrintf(CALLBACK_OUTPUT, "Response Code: %s\n", responseText);


		if(isoutput == 1){
			CHAR szBuffer[1025];
	    		DWORD dwRead=0;

		    while(WININET$InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer)-1, &dwRead) && dwRead) {
				szBuffer[dwRead] = 0;
				BeaconPrintf(CALLBACK_OUTPUT, "%s", szBuffer);
			    dwRead=0;
		    }

		}
    }
    WININET$InternetCloseHandle(hHttpRequest);
    WININET$InternetCloseHandle(hHttpSession);
    WININET$InternetCloseHandle(hIntSession);
}

void curl(char * host, char * trimmed_host, int port, int tls, char * method, int isoutput, char * useragent, char * headers, char * body, int proxy){
    int uri_element = 0;
    char * token = NULL;
    char * chunk;
    char uri[1000] = "";
    const char s[2] = "/"; //delimiter

    token = MSVCRT$strtok(host, s);

    // process the URL string to discern TLS, host and URI
    while( token != NULL ) {
    	if(MSVCRT$strcmp(token, "http:") == 0){
    		if (port == 0) { port = 80; }
    		tls = 0;
    		uri_element++;
    	} else if(MSVCRT$strcmp(token, "https:") == 0){
    		if (port == 0) { port = 443; }
    		tls = 1;
    		uri_element++;
    	} else if(uri_element == 2){
    		trimmed_host = token;
	    } else if (uri_element > 2) {
	    	MSVCRT$strcat(uri, s);
	    	MSVCRT$strcat(uri, token);
	    }
        token = MSVCRT$strtok(NULL, s);
	    uri_element++;
    }

    // we'll assume port 80 unless we know otherwise, just to make usage simple
    if (port == 0) { port = 80; }
 
    BeaconPrintf(CALLBACK_OUTPUT, "%s %s:%i %s\nUser Agent: %s\n%s\n", method, trimmed_host, port, uri, useragent, headers);
   	
    sendHttpRequest(method, trimmed_host, uri, port, useragent, headers, body, tls, isoutput, proxy);
}

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	//input param
	datap parser;	
	BeaconDataParse(&parser, Buffer, Length);
		
	char * host;
	char * trimmed_host;
	int port = 0;
	int tls = 0;
	char * method; 
	int isoutput;
	char * useragent;
	char * headers;
	char * body;
	int proxy;

	host = trimmed_host = BeaconDataExtract(&parser, NULL);
	port = BeaconDataInt(&parser);
	method = BeaconDataExtract(&parser, NULL);
	isoutput = BeaconDataInt(&parser);
	useragent = BeaconDataExtract(&parser, NULL);
	headers = BeaconDataExtract(&parser, NULL);
	body = BeaconDataExtract(&parser, NULL);
	proxy = BeaconDataInt(&parser);
	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	curl(host, trimmed_host, port, tls, method, isoutput, useragent, headers, body, proxy);
	printoutput(TRUE);
};

#else

int main(int argc, char ** argv)
{
	char * host = argv[1];
	char * trimmed_host = argv[2];
	int port = argv[3];
	int tls = argv[4];
	char * method = argv[5];
	int isoutput = argv[6];
	char * useragent = argv[7];
	char * headers = argv[8];
	char * body = argv[9];
	int proxy = argv[10];

	//code for standalone exe for scanbuild / leak checks	
	curl(host, trimmed_host, port, tls, method, isoutput, useragent, headers, body, proxy);
	return 0;
}

#endif
