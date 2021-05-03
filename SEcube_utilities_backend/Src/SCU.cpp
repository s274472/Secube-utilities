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

using namespace std;

unique_ptr<L0> l0;
unique_ptr<L1> l1;

int main(int argc, char *argv[]) {
	l0 = make_unique<L0>();
	l1 = make_unique<L1>();
	array<uint8_t, L1Parameters::Size::PIN> new_pin;

	char pin;
	char path;
	uint32_t keyID;
	string alg;
	utility utility;

	if(!parse_args(argc, argv, &pin, &utility, &path, &keyID, &alg)){
		cout << "[ERROR] Invalid command line arguments" << endl;
		return -1;
	}

	std::string s(1,path);

	login({'t','e','s','t'}, 0);
	/* Actions */
				switch (utility) {
				case ENCRYPTION:
					encryption(s, keyID, alg);
					break;
				case DECRYPTION:
					decryption(s);
					break;
				case DIGEST:
					digest(s, keyID, alg); //algorithms : 0) SHA-256 (no key required) 1) HMAC-SHA-256
					break;
				default:
					logout();
					break;
				}
		logout();
	return 0;
}

