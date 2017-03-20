#include "HTTPClientSample.h"

char *url = "http://codeforces.com/api/user.status?handle=Ferris&form=1&count=1";
int main(int argc, CHAR *argv[])
{

    INT32                   nRetCode;
    UINT32                  nSize;
    CHAR                    Buffer[HTTP_CLIENT_BUFFER_SIZE];
    HTTP_SESSION_HANDLE     pHTTP;

	// Open the HTTP request handle
	pHTTP = HTTPClientOpenRequest(0);

	// Set the Verb
	if((nRetCode = HTTPClientSetVerb(pHTTP,VerbGet)) != HTTP_CLIENT_SUCCESS){
		goto failed;
	}

	// Send a request for the home page 
	nRetCode = HTTPClientSendRequest(pHTTP, url, NULL, 0, FALSE, 0, 0);
	if(nRetCode  != HTTP_CLIENT_SUCCESS) {
		goto failed;
	}

	// Retrieve the the headers and analyze them
	if((nRetCode = HTTPClientRecvResponse(pHTTP, 2)) != HTTP_CLIENT_SUCCESS) {
		printf("HTTP receive error : %d\n", (int)nRetCode);
		goto failed;
	}

	while(nRetCode == HTTP_CLIENT_SUCCESS || nRetCode != HTTP_CLIENT_EOS){
		// Set the size of our buffer
		nSize = HTTP_CLIENT_BUFFER_SIZE;   
		// Get the data
		nRetCode = HTTPClientReadData(pHTTP, Buffer, nSize, 3, &nSize);
		if(nRetCode != HTTP_CLIENT_SUCCESS && nRetCode != HTTP_CLIENT_EOS) {
			printf("HTTP receive error when reading: %d\n", (int)nRetCode);
			goto failed;
		}
		// Print out the results
		printf("Total size : %ld\n%s\n", nSize, Buffer);
	}
	HTTPClientCloseRequest(&pHTTP);
	return 0;
failed:
	return -1;
}
