#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

#include "../Inc/decryption.h"
#include "../Inc/encryption.h"
#include "../Inc/login.h"
#include "../Inc/logout.h"
#include "../sefile/environment.h"
#include "../sefile/SEfile.h"
#include "../sqlite/sqlite3.h"

using namespace std;

unique_ptr<L0> l0 = make_unique<L0>();
unique_ptr<L1> l1 = make_unique<L1>();

int main() {
	int sel, err;

	cout << "Welcome to SEcube utilities! Please Login to your SEcube device."
			<< endl;
	/* Login */
	if(login({'t','e','s','t'}) != 0) {
		return -1;
	}

	/* Actions */
	do {
		err = 0;
		do {
			if (err != 0) {
				cerr << "Invalid number, please retry!" << endl;
				this_thread::sleep_for(chrono::milliseconds(1000));
			}
			cout << "\nSelect one of the actions below..." << endl;
			cout << "1. Encrypt a file" << endl;
			cout << "2. Decrypt a file" << endl;
			cout << "3. Digest a file" << endl;
			cout << "4. Quit" << endl;
			cin >> sel;
			if ((!sel) || sel < 1 || sel > 4) {
				err = 1;
			} else {
				err = 0;
			}
		} while (err != 0);

		switch (sel) {
		case 1:
			encryption("prova1.txt",811, "AES_HMACSHA256");
			break;
		case 2:
			decryption("prova1.txt");
			break;
		case 3:
			//digest
			break;
		default:
			/* Logout */
			logout();
			break;
		}
	} while (sel != 4);
	return 0;
}
