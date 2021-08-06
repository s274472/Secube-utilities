#include "../Inc/GUI_interface.h"

int network(int listenPort){

	cout << "[LOG] [Server] GUI Socket Server started." << endl;

	// Initialize Winsock:
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if( wsOk != 0 ) {
		cout << "[LOG] [Server] Error initializing WinSock!" << endl;
		return -1;
	}

	// Create a socket:
	int s0 = socket(AF_INET, SOCK_STREAM, 0);
	if (s0 < 0) {
		cout << "[LOG] [Server] Error creating socket!" << endl;
		return -1;
	}

	// Bind ip-address and port to socket:
	struct sockaddr_in address;
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons(listenPort);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	int res = bind(s0, (struct sockaddr*) &address, sizeof(address));
	if (res < 0) {
		cout << "[LOG] [Server] Error binding socket!" << endl;
		return -1;
	}

	// Tell winsock the socket is for listening
	res = listen(s0, 1); // "1" is the maximal length of the queue
	if (res < 0) {
		cout << "[LOG] [Server] Error listening!" << endl;
		return -1;
	}

	// Wait for connection
	struct sockaddr_in peer_address;
	socklen_t peer_address_len;
	cout << "[LOG] [Server] Waiting for connection..." << endl;
	int s1 = accept(s0, NULL, NULL);
	if (s1 < 0) {
		cout << "[LOG] [Server] accept failed: %d\n" << WSAGetLastError() << endl;
		return -1;
	}
	cout << "[LOG] [Server] Connected to GUI." << endl;

	// Close listening socket
	closesocket(s0);

	return s1;
}

void closeAndCleanConnection(int sock) {

	// Close the socket:
	closesocket(sock);
	// Cleanup winsock:
	WSACleanup();

	return;
}
