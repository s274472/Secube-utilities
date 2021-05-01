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

int main() {
	int sel, err;
	l0 = make_unique<L0>();
	l1 = make_unique<L1>();

	cout << "Welcome to SEcube utilities!"
			<< endl;
	/* Login */
	if(login({'t','e','s','t'}) != 0) {
		return -1;
	}

	/* Actions */
		switch (sel) {
		case 1:
			//login
			encryption("/home/user/Desktop/sefile_example.txt", 978, "AES_HMACSHA256");
			//logout
			break;
		case 2:
			//login
			decryption("/home/user/Desktop/sefile_example.txt");
			//logout
			break;
		case 3:
			//login
			digest("prova1.txt", 983, "HMAC-SHA-256"); //algorithms : 0) SHA-256 (no key required) 1) HMAC-SHA-256
			//logout
			break;
		default:
			/* Logout */
			logout();
			break;
		}
	return 0;
}
