#ifdef _WIN32

#ifndef INC_GUI_INTERFACE_H_
#define INC_GUI_INTERFACE_H_

#include <ws2tcpip.h>
#include <iostream>
#include "../cereal/archives/binary.hpp"

using namespace std;

#define BUFLEN 1024*100 // dimension of input/output buffer for communicating with the GUI
#define STR_SIZE 250
#define ARR_SIZE 20
#define comm_port 1235

// Global variable for allowing the backend to work as a server for the GUI
// The content of this variable is handled by the argument parser
extern int gui_server_on;

// Structs sent to the GUI containing the response:
// Each Struct is associated to a specific utility function

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

// Connect to the GUI via socket:
int network(int listenPort);

// Clean socket connection
void closeAndCleanConnection(int sock);

// Used when only an error code and and error message must be sent to the GUI:
template <class Response>
void sendErrorToGUI(int sock, Response resp, int err_code, string err_msg) {

	std::stringstream ss; // any stream can be used

	{
		cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

		// Prepare response to GUI:
		resp.err_code = err_code;
		strcpy(resp.err_msg, err_msg.c_str());
		oarchive(resp);

	} // archive goes out of scope, ensuring all contents are flushed

	// Send response to GUI:
	send(sock, ss.str().c_str(), ss.str().length(), 0);

	return;
}

template <class Response>
void sendResponseToGUI(int sock, Response resp) {

	std::stringstream ss; // any stream can be used

	{
		cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

		// Prepare response to GUI:
		oarchive(resp);

	} // archive goes out of scope, ensuring all contents are flushed

	// Send response to GUI:
	send(sock, ss.str().c_str(), ss.str().length(), 0);

	return;
}


#endif /* INC_GUI_INTERFACE_H_ */
#endif
