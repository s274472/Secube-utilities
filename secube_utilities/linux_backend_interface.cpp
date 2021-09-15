#ifdef __linux__

#include "linux_backend_interface.h"

Backend_Interface::Backend_Interface()
{

}

int connectToBackend() {

    cout << "[LOG] [Client] GUI is connecting to backend server..." << endl;

    // Create a socket:
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "[LOG] [Client] Error creating socket!" << endl;
        return -1;
    }

    int valread;
    struct sockaddr_in serv_addr;
    uint8_t buffer[BUFLEN];


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(comm_port);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
        cout << "[Client] [Client] Invalid address / Address not supported" << endl;
        return -1;
    }
    int i = 0;
    while(true){
        i++;
        if (::connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            cout << "[LOG] [Client] Connection failed" << endl;
            if(i>=10){
                return -1;
            }
        } else {break;}
    }

    cout << "[LOG] [Client] Connected to backend!" << endl;

    return sock;
}
#endif
