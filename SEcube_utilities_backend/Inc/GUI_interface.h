#ifdef _WIN32 // This source is compiled only on Windows

#ifndef INC_GUI_INTERFACE_H_
#define INC_GUI_INTERFACE_H_

#include <ws2tcpip.h>
#include <iostream>
#include "../cereal/archives/binary.hpp"

using namespace std;

#define BUFLEN 1024*100 // dimension of input/output buffer for communicating with the GUI
#define STR_SIZE 250
#define ARR_SIZE 20
#define comm_port 1235 // The port used for the socket connection to the GUI

// Global variable for allowing the backend to work as a server for the GUI
// The content of this variable is handled by the argument parser
extern int gui_server_on;

/**
 * When the backend is started with the gui_server argument, after performing the desired utility it waits for a socket connection with the GUI and then
 * sends back the outcome of the utility operation performed.
 * The information sent back can be simple(for example: everything was correct or some error arise) or more complex(informations of all the SECube
 * devices attached to the PC).
 *
 * Each utility has a specific struct as response and the GUI is expecting to receive exactly that particular struct.
 *
 * Here it is listed for all the utilities, which struct they are using as response:
 * digest -> Response_GENERIC; encryption -> Response_GENERIC; decryption -> Response_GENERIC;
 * list_devices -> Response_DEV_LIST; list_keys -> Response_LIST_KEYS; update_SEKey_path -> Response_GENERIC;
 *
 * In order to send the struct via the socket connection a serialization library, Cereal, is used. The Response Struct is serialized, sent to the GUI
 * via socket connection and then the GUI will deserialize the struct using Cereal again.
 *
 */

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
  uint16_t key_sizes[ARR_SIZE]; // An array of uint16_t (key_sizes), for storing the Key Size. The size is in bits

  // This method lets cereal know which data members to serialize
  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( err_code, err_msg, num_keys, key_ids, key_sizes ); // serialize things by passing them to the archive
  }
};

int network(int listenPort);

void closeAndCleanConnection(int sock);

/**
 * This function is used to easily send a response to the GUI via socket containing only err_msg and err_code.
 * To correctly use this function, specify which kind of Response Struct to use matching the one that the GUI is expecting.
 *
 * returns: void
 */
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

/**
 * This function is used to send a Response Struct(can be any kind of Response Struct) to the GUI via socket.
 * To correctly use this function, specify which kind of Response Struct to use matching the one that the GUI is expecting.
 *
 * returns: void
 */
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
