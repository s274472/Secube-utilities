#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>>
#include <sstream>
#include "cereal/archives/binary.hpp"

#define BUFLEN 1024*100 // dimension of input/output buffer for communicating with the GUI
#define comm_port 1235
#define STR_SIZE 50
#define ARR_SIZE 20

using namespace std;

class Backend_Interface
{
public:
    Backend_Interface();
};

struct Response_GENERIC {

    int err_code;
    char err_msg[STR_SIZE];

    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive & archive)
    {
      archive( err_code, err_msg ); // serialize things by passing them to the archive
    }
};
struct Response_DEV_LIST : Response_GENERIC
{

  int num_devices;
  char paths[ARR_SIZE][STR_SIZE];
  char serials[ARR_SIZE][STR_SIZE];

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( err_code, err_msg, num_devices, paths, serials ); // serialize things by passing them to the archive
  }
};

struct Response_LIST_KEYS : Response_GENERIC
{

  int num_keys;
  uint32_t key_ids[ARR_SIZE];
  uint16_t key_sizes[ARR_SIZE]; // Size is in bits

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( err_code, err_msg, num_keys, key_ids, key_sizes ); // serialize things by passing them to the archive
  }
};

int connectToBackend();

template <class Response>
Response sendRequestToBackend(string cmd) {

    Response resp;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    CreateProcessA("secube_cmd.exe", LPSTR(cmd.c_str()), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    // Connect to backend server:
    int sock = connectToBackend();

    // Wait for response from backend:
    bool quit = false;
    int res = -1;
    char request[BUFLEN] = {0};
    char reply[BUFLEN] = {0};
    std::stringstream ss; // any stream can be used

    while (!quit) {
        memset(request, 0, BUFLEN);
        memset(reply, 0, BUFLEN);
        res = recv(sock, request, BUFLEN, 0);
        if (res < 0) {
            cout << "[LOG] [Client] Error reading response from backend!" << endl;
            quit=true;
        } else { // process request depending on type
            cout << "[LOG] [Client] Received " << res << " bytes." << endl;

            std::stringstream ss;
            ss.write((char*)request, res);
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(resp); // Read the data from the archive

            quit = true;
        }
    }

    // Close the socket:
    closesocket(sock);

    // Cleanup winsock:
    WSACleanup();

    cout << "[LOG] [Client] Disconnected." << endl;

    return resp;
}

#endif // BACKEND_INTERFACE_H
