#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

#include "../Inc/decryption.h"
#include "../Inc/encryption.h"
#include "../Inc/digest.h"
#include "../Inc/utilities.h"
#include "../sefile/environment.h"
#include "../sefile/SEfile.h"
#include "../sqlite/sqlite3.h"

typedef unsigned int uint;

using namespace std;

unique_ptr<L0> l0;
unique_ptr<L1> l1;

int main(int argc, char *argv[]) {
	l0 = make_unique<L0>();
	l1 = make_unique<L1>();
	array<uint8_t, L1Parameters::Size::PIN> new_pin;

	int deviceID;
	string pin;
	string path;
	uint32_t keyID;
	string alg;
	utility utility;

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
		//Decryption
		if (strcmp(argv[cur], "-d") == 0) {
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
		login(new_pin, deviceID);
		encryption(path, keyID, alg);
		logout();
		break;
	case DECRYPTION:
		login(new_pin, deviceID);
		decryption(path);
		logout();
		break;
	case DIGEST:
		login(new_pin, deviceID);
		digest(path, keyID, alg); //algorithms : 0) SHA-256 (no key required) 1) HMAC-SHA-256
		logout();
		break;
	case DEV_LIST:
		list_devices();
		break;
	case K_LIST:
		login(new_pin, deviceID);
		list_keys();
		logout();
		break;
	default:
		cout << "[ERROR] Something went wrong! Quit." << endl;
		return -1;
		break;
	}
	return 0;
}

