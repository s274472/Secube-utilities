#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>>

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
  int key_ids[ARR_SIZE];
  int key_sizes[ARR_SIZE]; // Size is in bits

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( err_code, err_msg, num_keys, key_ids, key_sizes ); // serialize things by passing them to the archive
  }
};

int connectToBackend();

#endif // BACKEND_INTERFACE_H
