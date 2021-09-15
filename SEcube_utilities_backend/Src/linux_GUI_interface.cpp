#ifdef __linux__

#include "../Inc/linux_GUI_interface.h"

int network(int listenPort){

	cout << "[LOG] [Server] GUI Socket Server started." << endl;

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

	int yes=1;
	if( setsockopt(s0, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0 ) {
		cout << "[LOG] [Server] setsockopt failed! Error: " << errno << endl;
	}

	int res = bind(s0, (struct sockaddr*) &address, sizeof(address));
	if (res < 0) {
		cout << "[LOG] [Server] Error binding socket! error number: " << errno << endl;
		return -1;
	}

	// Tell the socket is for listening
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
		cout << "[LOG] [Server] accept failed: %d\n" << errno << endl;
		return -1;
	}
	cout << "[LOG] [Server] Connected to GUI." << endl;

	// Close listening socket
	close(s0);

	return s1;
}

void closeAndCleanConnection(int sock) {

	// Close the socket:
	close(sock);

	return;
}

#endif
