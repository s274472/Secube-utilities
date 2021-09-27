#ifdef _WIN32 // This source is compiled only on Windows

#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

// This defines must be added otherwise on Windows, during compilation, the function inet_pton was not found:
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000

#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include "cereal/archives/binary.hpp"

#define BUFLEN 1024*100 // dimension of input/output buffer for communicating with the Backend
#define STR_SIZE 300
#define ARR_SIZE 20
#define comm_port 1235 // The port used for the socket connection to the Backend

using namespace std;

class Backend_Interface
{
public:
    Backend_Interface();
};

// Response Structs:

/**
 * The most basic Response Struct. It contains only the error_code and the error_message.
 * If this struct is used for sending a "everything was correct" message, the error_code must be set to 0.
 *
 * All the other Response Struct must inherit this one(because all the responses must have an err_code and err_msg)
 */
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

/**
 * Response struct used by the list_devices utility.
 */
struct Response_DEV_LIST : Response_GENERIC
{

  int num_devices; // The number of SECube devices connected to the PC
  char paths[ARR_SIZE][STR_SIZE]; // An array of string, for storing the device path
  char serials[ARR_SIZE][STR_SIZE]; // An array of string, for storing the device serial

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( err_code, err_msg, num_devices, paths, serials ); // serialize things by passing them to the archive
  }
};

/**
 * Response struct used by the list_keys utility.
 */
struct Response_LIST_KEYS : Response_GENERIC
{

  int num_keys; // Number of keys stored in the selected SECube device
  uint32_t key_ids[ARR_SIZE]; // An array of uint32_t, for storing the KeyID
  uint16_t key_sizes[ARR_SIZE]; // An array of uint16_t, for storing the Key Size. The size is in bits

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( err_code, err_msg, num_keys, key_ids, key_sizes ); // serialize things by passing them to the archive
  }
};

int connectToBackend();

/**
 * This function creates a process in background for running the Backend application. The utility function performed by the
 * backend depends on the cmd input parameter.
 * After creating the Backend process and connecting to it via socket, this function waits for a Response from the Backend.
 *
 * This is a generic function, the Response Template must be expanded with the right Response Struct(this depends on the utlity to perform).
 *
 * Returns: Response of the utility function performed by the Backend
 */
template <class Response>
Response sendRequestToBackend(string cmd) {

    Response resp;

    // Create a process in background for running the Backend application using the cmd received:
    // In this way in background the Backend will perform the utility function and then will send the response to the GUI
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( CreateProcessA("secube_cmd.exe", LPSTR(cmd.c_str()), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi) == 0 ) {
        // In case of error tryng to run the Backend application:

        // Create and return an error Response:
        resp.err_code = -1;
        string err_msg = "The Backend application cannot not be started!";
        strcpy(resp.err_msg, err_msg.c_str());
    }
    else { // Backend correctly started:

        // Connect to the backend via socket:
        int sock = connectToBackend();

        // Wait for response from the Backend:
        int res = -1;
        char request[BUFLEN] = {0};
        char reply[BUFLEN] = {0};
        std::stringstream ss; // any stream can be used

        memset(request, 0, BUFLEN);
        memset(reply, 0, BUFLEN);
        res = recv(sock, request, BUFLEN, 0);
        if (res < 0) {
            cout << "[LOG] [GUI] Error reading response from backend!" << endl;
        } else {
            cout << "[LOG] [GUI] Received " << res << " bytes." << endl;

            // Deserialize the Response using Cereal:
            std::stringstream ss;
            ss.write((char*)request, res);
            cereal::BinaryInputArchive iarchive(ss);
            iarchive(resp); // Read the data from the archive
        }


        // Close the socket:
        closesocket(sock);

        // Cleanup winsock:
        WSACleanup();

        cout << "[LOG] [GUI] Disconnected." << endl;
    }

    return resp;
}

#endif // BACKEND_INTERFACE_H
#endif
