#ifdef _WIN32 // This source is compiled only on Windows

#include "../Inc/GUI_interface.h"

/**
 * This function waits for a connection with the GUI via socket on the provided listenPort.
 *
 * returns: socket ID
 */
int network(int listenPort){

	cout << "[LOG] [Backend] GUI Socket Server started." << endl;

	// Initialize Winsock:
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if( wsOk != 0 ) {
		cout << "[LOG] [Backend] Error initializing WinSock!" << endl;
		return -1;
	}

	// Create a socket:
	int s0 = socket(AF_INET, SOCK_STREAM, 0);
	if (s0 < 0) {
		cout << "[LOG] [Backend] Error creating socket!" << endl;
		return -1;
	}

	// Bind ip-address and port to socket:
	struct sockaddr_in address;
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons(listenPort);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");

	int yes=1;
	if( setsockopt(s0, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0 ) {
		cout << "[LOG] [Backend] setsockopt failed! Error: " << errno << endl;
	}

	int res = bind(s0, (struct sockaddr*) &address, sizeof(address));
	if (res < 0) {
		cout << "[LOG] [Backend] Error binding socket! error number: " << errno << endl;
		return -1;
	}

	// Tell winsock the socket is for listening
	res = listen(s0, 1); // "1" is the maximal length of the queue
	if (res < 0) {
		cout << "[LOG] [Backend] Error listening!" << endl;
		return -1;
	}

	// Wait for connection
	cout << "[LOG] [Backend] Waiting for connection..." << endl;
	int s1 = accept(s0, NULL, NULL);
	if (s1 < 0) {
		cout << "[LOG] [Backend] accept failed: %d\n" << WSAGetLastError() << endl;
		return -1;
	}
	cout << "[LOG] [Backend] Connected to GUI." << endl;

	// Close listening socket
	closesocket(s0);

	return s1;
}

/**
 * This functions closes and cleans the socket connection.
 *
 * returns: void
 */
void closeAndCleanConnection(int sock) {

	// Close the socket:
	closesocket(sock);
	// Cleanup winsock:
	WSACleanup();

	return;
}

#endif
