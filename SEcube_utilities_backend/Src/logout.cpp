#include "../Inc/logout.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

int logout() {

	cout << "\nLogging out..." << endl;
	try {
		l1->L1Logout();
	} catch (...) {
		cout << "Logout error. Quit." << endl;
		return -1;
	}
	if (l1->L1GetSessionLoggedIn()) { // check if logout was ok
		cout << "Logout error. Quit." << endl;
		return -1;
	}
	cout << "You are now logged out." << endl;

	return 0;
}
