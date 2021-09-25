/**
 * Entry point of the SEcube Utilities Backend application.
 * Contains the main function, that extracts all the input parameters and perform the requested security utility.
 *
 * In case the gui_server flag is provided, all the output messages are forwarded to the GUI via socket.
 */

#include <thread>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "../Inc/utilities.h"
#include "../sefile/environment.h"
#include "../sefile/SEfile.h"
#include "../sqlite/sqlite3.h"

#include "../cereal/archives/binary.hpp"

typedef unsigned int uint;

using namespace std;

unique_ptr<L0> l0;
unique_ptr<L1> l1;

// Global variable for allowing the backend to work as a server for the GUI
// The content of this variable is handled by the argument parser
int gui_server_on = false; // If true the GUI server is on

int main(int argc, char *argv[]) {
	l0 = make_unique<L0>();
	l1 = make_unique<L1>();
	array<uint8_t, L1Parameters::Size::PIN> new_pin;

	// Parameters, used for all the utilities:
	bool decrypt_with_sekey = false;
	int deviceID;
	string update_path;
	string pin;
	string user;
	string group;
	string path;
	uint32_t keyID = 0;
	string alg;
	Utilities utility = DEFAULT;
	std::array<uint8_t, B5_SHA256_DIGEST_SIZE> nonce;nonce.fill(0);
	bool usenonce = false;

	int gui_socket = -1; // Socket used for GUI interfacing

	// Input flags parser, extracts all the provided parameters and calls the specified utility:
	int cur = 1;
	while (cur < argc) {
		//Help
		if (strcmp(argv[cur], "-help") == 0) {
			print_command_line();
			return 0;
		}
		//Device
		if (strcmp(argv[cur], "-dev") == 0) {
			if (argc > cur + 1) {
				deviceID = atoi(argv[++cur]);
			} else {
				cout << "Error! no device specified after -dev! Quit." << endl;
				return -1;
			}
		}
		//Pin
		if (strcmp(argv[cur], "-p") == 0) {
			if (argc > cur + 1) {
				pin = argv[++cur];
			} else {
				cout << "Error! no pin specified after -p! Quit." << endl;
				return -1;
			}
		}
		//Encryption
		if (strcmp(argv[cur], "-e") == 0) {
			utility = ENCRYPTION;
		}
		//Decryption manual
		if (strcmp(argv[cur], "-d") == 0) {
			decrypt_with_sekey = false;
			utility = DECRYPTION;
		}
		//Decryption with sekey
		if (strcmp(argv[cur], "-dk") == 0) {
			decrypt_with_sekey = true;
			utility = DECRYPTION;
		}
		//Digest
		if (strcmp(argv[cur], "-di") == 0) {
			utility = DIGEST;
		}
		//Devices list
		if (strcmp(argv[cur], "-dl") == 0) {
			utility = DEV_LIST;
		}
		//Keys list
		if (strcmp(argv[cur], "-kl") == 0) {
			utility = K_LIST;
		}
		//Update path
		if (strcmp(argv[cur], "-update_path") == 0) {
			utility = UPDATE_PATH;
			if (argc > cur + 1) {
				update_path = argv[++cur];
			} else {
                cout << "Error! no path specified after -update_path! Quit." << endl;
				return -1;
			}
		}
		//Gui Server
		if (strcmp(argv[cur], "-gui_server") == 0) {
			gui_server_on = true;
		}
		//Nonce
		if (strcmp(argv[cur], "-nonce") == 0) {
			usenonce = true;
			char* nonce_str = argv[++cur];
			for(int i=0; (i<strlen(nonce_str) && i<B5_SHA256_DIGEST_SIZE); i++) { // If the nonce string size in input is greater than B5_SHA256_DIGEST_SIZE,
				nonce[i] = nonce_str[i];										  // the string is truncated
			}
		}
		//User(s) ID(s)
		if (strcmp(argv[cur], "-u") == 0) {
			if (argc > cur + 1) {
				user = argv[++cur];
			} else {
				cout << "Error! no user specified after -u! Quit." << endl;
				return -1;
			}
		}
		//Group ID
		if (strcmp(argv[cur], "-g") == 0) {
			if (argc > cur + 1) {
				group = argv[++cur];
			} else {
				cout << "Error! no group specified after -g! Quit." << endl;
				return -1;
			}
		}
		//Filename path
		if (strcmp(argv[cur], "-f") == 0) {
			if (argc > cur + 1) {
				path = argv[++cur];
			} else {
				cout << "Error! no file specified after -f! Quit." << endl;
				return -1;
			}
		}
		//Key ID
		if (strcmp(argv[cur], "-k") == 0) {
			if (argc > cur + 1) {
				keyID = ((uint32_t) atoi(argv[++cur]));
			} else {
				cout << "Error! no key specified after -k! Quit." << endl;
				return -1;
			}
		}
		//Algorithm
		if (strcmp(argv[cur], "-aes_hmac") == 0) {
			alg = "AES_HMACSHA256";
		} else if (strcmp(argv[cur], "-sha") == 0) {
			alg = "SHA-256";
		} else if (strcmp(argv[cur], "-hmac") == 0) {
			alg = "HMAC-SHA-256";
		} else if (strcmp(argv[cur], "-aes") == 0) {
			alg = "AES";
		}
		cur++;
	}
	for (uint i = 0; i < new_pin.size(); i++) {
		if (i < pin.size())
			new_pin[i] = (uint8_t) pin[i];
		else
			new_pin[i] = '\0';
	}

	/* Actions */
	switch (utility) {
		case ENCRYPTION:

			// Connect to the GUI:
			if(gui_server_on){
				gui_socket = network(comm_port);
			}

			// Login:
			{
				int err = login(new_pin, deviceID);
				if( err<0 ) { // In case of error during login:

					// For GUI interfacing:
					if(gui_server_on){
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error during login!");
					}

					return -1;
				}
			}

			if (keyID != 0) {// If a keyID is specified, the encryption utility can be called

				// Check keyID is contained inside the SECube device:
				if( isKeyContained(keyID) )
					encryption(gui_socket, path, keyID, alg);
				else {
					cout << "The specified keyID is not contained inside the selected SECube device! Quit." << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "The specified keyID is not contained inside the selected SECube device!");
					}

					return -1;
				}
			}
			else { // If a keyID is not specified, SEKey is used for finding an usable key for the specified user(s) or group

				// Check if the sekey_path must be updated:
				if (!read_sekey_update_path(*l0.get(), l1.get())) {
					cout << "Update the sekey path!" << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Update the sekey path!");
					}

					return -1;
				}

				// Use SEKey to find an usable key for the specified user(s) or group:
				if (find_key(keyID, user, group)) {

					if(sekey_start(*l0, l1.get()) != 0){ // In case of error starting SEKey:
						cout << "Error starting SEkey!" << endl;

						// For GUI interfacing:
						if(gui_server_on) {
							Response_GENERIC resp;
							sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error starting SEkey!");
						}

						return -1;
					}
					encryption(gui_socket, path, keyID, alg); // The encryption utility is called with the KeyID obtained using SEKey
					sekey_stop();
				}
				else {
					cout << "SEkey: No valid key found!" << endl;
					if (gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "SEkey: No valid key found!");
					}
					return -1;
				}
			}

			logout();

			// Clean GUI connection:
			if(gui_server_on){
				closeAndCleanConnection(gui_socket);
			}

			break;
		case DECRYPTION:

			// Connect to the GUI:
			if(gui_server_on){
				gui_socket = network(comm_port);
			}

			// Login:
			{
				int err = login(new_pin, deviceID);
				if( err<0 ) { // In case of error during login:

					// For GUI interfacing:
					if(gui_server_on){
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error during login!");
					}

					return -1;
				}
			}

			if (decrypt_with_sekey) { // In case the decryption must be done using SEKey, SEKey needs to be started:

				// Check if the sekey_path must be updated:
				if (!read_sekey_update_path(*l0.get(), l1.get())) {
					cout << "Update the sekey path!" << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Update the sekey path!");
					}

					return -1;
				}

				if(sekey_start(*l0, l1.get()) != 0){ // In case of error starting SEKey:
					cout << "Error starting SEkey!" << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error starting SEkey!");
					}

					return -1;
				}
			}

			decryption_w_encrypted_filename(gui_socket, path); // The decryption utility is called

			if (decrypt_with_sekey)
				sekey_stop();

			logout();

			// Clean GUI connection:
			if(gui_server_on){
				closeAndCleanConnection(gui_socket);
			}

			break;
		case DIGEST:

			// Connect to the GUI:
			if(gui_server_on){
				gui_socket = network(comm_port);
			}

			// Login:
			{
				int err = login(new_pin, deviceID);
				if( err<0 ) { // In case of error during login:

					// For GUI interfacing:
					if(gui_server_on){
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error during login!");
					}

					return -1;
				}
			}

			// The SHA-256 algorithm do not require a key
			// The HMAC-SHA-256 requires a key, this must be or manually inserted or retrieved using SEKey
			if ( (alg.compare("SHA-256") == 0)  ){  // If the key is not needed, proceed:
				digest(gui_socket, path, keyID, alg, usenonce, nonce); // The digest utility is called
			}
			else if ( (keyID!=0) ) { // If the key is needed and manually inserted:
				// Check keyID is contained inside the SECube device:
				if( isKeyContained(keyID) )
					digest(gui_socket, path, keyID, alg, usenonce, nonce); // The digest utility is called
				else {
					cout << "The specified keyID is not contained inside the selected SECube device! Quit." << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "The specified keyID is not contained inside the selected SECube device!");
					}

					return -1;
				}
			}
			else { // Else extract the key using SEKey:

				// Check if the sekey_path must be updated:
				if (!read_sekey_update_path(*l0.get(), l1.get())) {
					cout << "Update the sekey path!" << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Update the sekey path!");
					}

					return -1;
				}

				// Use SEKey to find an usable key for the specified user(s) or group:
				if (find_key(keyID, user, group)) {
					if(sekey_start(*l0, l1.get()) != 0){ // In case of error starting SEKey:
						cout << "Error starting SEkey!" << endl;

						// For GUI interfacing:
						if(gui_server_on) {
							Response_GENERIC resp;
							sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error starting SEkey!");
						}

						return -1;
					}
					digest(gui_socket, path, keyID, alg, usenonce, nonce); // The digest utility is called with the KeyID obtained using SEKey
					sekey_stop();
				}
				else {
					cout << "SEkey: No valid key found!" << endl;
					if (gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "SEkey: No valid key found!");
					}
					return -1;
				}
			}

			logout();

			// Clean GUI connection:
			if(gui_server_on){
				closeAndCleanConnection(gui_socket);
			}

			break;
		case DEV_LIST:

			// Connect to the GUI:
			if(gui_server_on){
				gui_socket = network(comm_port);
			}

			list_devices(gui_socket);

			// Clean GUI connection:
			if(gui_server_on){
				closeAndCleanConnection(gui_socket);
			}

			break;
		case K_LIST:

			// Connect to the GUI:
			if(gui_server_on){
				gui_socket = network(comm_port);
			}

			// Login:
			{
				int err = login(new_pin, deviceID);
				if( err<0 ) { // In case of error during login:

					// For GUI interfacing:
					if(gui_server_on){
						Response_LIST_KEYS resp;
						sendErrorToGUI<Response_LIST_KEYS>(gui_socket, resp, -1, "Error during login!");
					}

					return -1;
				}
			}

			list_keys(gui_socket);
			logout();

			// Clean GUI connection:
			if(gui_server_on){
				closeAndCleanConnection(gui_socket);
			}

			break;
		case UPDATE_PATH:

			// Connect to the GUI:
			if(gui_server_on){
				gui_socket = network(comm_port);
			}

			// Login:
			{
				int err = login(new_pin, deviceID);
				if( err<0 ) { // In case of error during login:

					// For GUI interfacing:
					if(gui_server_on){
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error during login!");
					}

					return -1;
				}
			}

			if (!set_sekey_update_path(update_path, *l0.get(), l1.get())) { // In case of error updating the SEKey path:
				cout << "Error setting the new path!" << endl;

				// For GUI interfacing:
				if(gui_server_on) {
					Response_GENERIC resp;
					sendErrorToGUI<Response_GENERIC>(gui_socket, resp, -1, "Error setting the new path!");
				}

				return -1;
			}

			cout << "Path correctly updated!" << endl;

			// For GUI interfacing:
			if(gui_server_on) {
				Response_GENERIC resp;
				sendErrorToGUI<Response_GENERIC>(gui_socket, resp, 0, "Path correctly updated!"); // In this case err_code = 0 means everything went correctly
			}

			logout();
			break;

		default:
			cout << "Error! Unknown utility specified! Please use -help for the list of available utilities." << endl;
			return -1;
			break;
		}

	return 0;
}
