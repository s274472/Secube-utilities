#ifdef _WIN32 // This source is compiled only on Windows

#include "backend_interface.h"

Backend_Interface::Backend_Interface()
{

}

/**
 * This function creates a connection to the Backend via socket on the provided commPort.
 *
 * returns: socket ID
 */
int connectToBackend() {

    cout << "[LOG] [GUI] GUI is connecting to backend server..." << endl;

    // Initialize Winsock:
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if( wsOk != 0 ) {
        cout << "[LOG] [GUI] Error initializing WinSock!" << endl;
        return -1;
    }

    // Create a socket:
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "[LOG] [GUI] Error creating socket!" << endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(comm_port);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        cout << "[LOG] [GUI] Invalid address / Address not supported" << endl;
        return -1;
    }

    // Try to connect to the Backend with maximum 10 attempt:
    int i = 0;
    while(true){
        i++;
        if (::connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            cout << "[LOG] [GUI] Connection failed" << endl;
            if(i>=10){
                return -1;
            }
        } else {break;}
    }

    cout << "[LOG] [GUI] Connected to backend!" << endl;

    return sock;
}

#endif
