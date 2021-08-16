#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "../Inc/decryption.h"
#include "../Inc/encryption.h"
#include "../Inc/digest.h"
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
// If true the GUI server is on
int gui_server_on = false;

int main(int argc, char *argv[]) {
	l0 = make_unique<L0>();
	l1 = make_unique<L1>();
	array<uint8_t, L1Parameters::Size::PIN> new_pin;

	bool decrypt_with_sekey = false;
	int deviceID;
	string update_path;
	string pin;
	string user;
	string group;
	string path;
	uint32_t keyID = 0;
	string alg;
	utility utility;

	int s1 = -1; // Socket used for GUI interfacing

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
			} else
				return 0;
		}
		//Pin
		if (strcmp(argv[cur], "-p") == 0) {
			if (argc > cur + 1) {
				pin = argv[++cur];
			} else
				return 0;
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
			} else
				return 0;
		}
		//Gui Server
		if (strcmp(argv[cur], "-gui_server") == 0) {
			//utility = GUI_SERVER;
			gui_server_on = true;
		}
		//User(s) ID(s)
		if (strcmp(argv[cur], "-u") == 0) {
			if (argc > cur + 1) {
				user = argv[++cur];
			} else
				return 0;
		}
		//Group ID
		if (strcmp(argv[cur], "-g") == 0) {
			if (argc > cur + 1) {
				group = argv[++cur];
			} else
				return 0;
		}
		//Filename path
		if (strcmp(argv[cur], "-f") == 0) {
			if (argc > cur + 1) {
				path = argv[++cur];
			} else
				return 0;
		}
		//Key ID
		if (strcmp(argv[cur], "-k") == 0) {
			if (argc > cur + 1) {
				keyID = ((uint32_t) atoi(argv[++cur]));
			} else
				return 0;
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
			s1 = network(comm_port);
		}

		{
			int err = login(new_pin, deviceID);
			// For GUI interfacing:
			if( (err<0) && (gui_server_on) ) {
				Response_GENERIC resp;
				sendErrorToGUI<Response_GENERIC>(s1, resp, -1, "Error during login!");
			}
		}

		if (keyID != 0)
			encryption(s1, path, keyID, alg);
		else {
			if (!read_sekey_update_path(*l0.get(), l1.get())) {
				cout << "Update the sekey path!" << endl;

				// For GUI interfacing:
				if(gui_server_on) {
					Response_GENERIC resp;
					sendErrorToGUI<Response_GENERIC>(s1, resp, -1, "Update the sekey path!");
				}

				return -1;
			}
			if (find_key(keyID, user, group)) {
				if(sekey_start(*l0, l1.get()) != 0){
					cout << "Error starting SEkey!" << endl;

					// For GUI interfacing:
					if(gui_server_on) {
						Response_GENERIC resp;
						sendErrorToGUI<Response_GENERIC>(s1, resp, -1, "Error starting SEkey!");
					}

					return -1;
				}
				encryption(s1, path, keyID, alg);
				sekey_stop();
			}
		}
		logout();

		// Clean GUI connection:
		if(gui_server_on){
			closeAndCleanConnection(s1);
		}

		break;
	case DECRYPTION:
		login(new_pin, deviceID);
		if (decrypt_with_sekey) {
			if (!read_sekey_update_path(*l0.get(), l1.get())) {
				cout << "Update the sekey path!" << endl;
				return -1;
			}
			if(sekey_start(*l0, l1.get()) != 0){
				cout << "Error starting SEkey!" << endl;
				return -1;
			}
		}
		decryption_w_encrypted_filename(path);
		if (decrypt_with_sekey)
			sekey_stop();
		logout();
		break;
	case DIGEST:
		login(new_pin, deviceID);
		if (keyID != 0)
		digest(path, keyID, alg); //algorithms : 0) SHA-256 (no key required) 1) HMAC-SHA-256
		else {
			if (!read_sekey_update_path(*l0.get(), l1.get())) {
				cout << "Update the sekey path!" << endl;
				return -1;
			}
			if (find_key(keyID, user, group)) {
				if(sekey_start(*l0, l1.get()) != 0){
					cout << "Error starting SEkey!" << endl;
					return -1;
				}
				digest(path, keyID, alg);
				sekey_stop();
			}
		}
		logout();
		break;
	case DEV_LIST:

		// Connect to the GUI:
		if(gui_server_on){
			s1 = network(comm_port);
		}

		list_devices(s1);

		// Clean GUI connection:
		if(gui_server_on){
			closeAndCleanConnection(s1);
		}

		break;
	case K_LIST:

		// Connect to the GUI:
		if(gui_server_on){
			s1 = network(comm_port);
		}

		{
			int err = login(new_pin, deviceID);
			// For GUI interfacing:
			if( (err<0) && (gui_server_on) ) {
				Response_LIST_KEYS resp;
				sendErrorToGUI<Response_LIST_KEYS>(s1, resp, -1, "Error during login!");
			}
		}

		list_keys(s1);
		logout();

		// Clean GUI connection:
		if(gui_server_on){
			closeAndCleanConnection(s1);
		}

		break;
	case UPDATE_PATH:
		login(new_pin, deviceID);
		if (!set_sekey_update_path(update_path, *l0.get(), l1.get())) {
			cout << "Error setting the new path!" << endl;
			return -1;
		}
		cout << "Path correctly updated!" << endl;
		logout();
		break;

	default:
		cout << "[ERROR] Something went wrong! Quit." << endl;
		return -1;
		break;
	}
	return 0;
}
