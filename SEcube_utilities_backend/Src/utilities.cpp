#include "../Inc/utilities.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

//Will return the number of devices found.
int list_devices() {

	cout << "Looking for SEcube devices..." << endl;
	this_thread::sleep_for(chrono::milliseconds(2000));

	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
	if (ret) {
		cerr << "\nError while searching for SEcube devices! Quit." << endl;
		return -1;
	}

	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
	if (numdevices == 0) {
		cerr << "\nNo SEcube devices found! Quit." << endl;
		return 0;
	}
	cout << "Number of SEcube devices found: " << numdevices << endl;
	int index = 0;
	cout << "------------------------------------------------------------------------------------" << endl;
	cout << "DevID" << "\t\tPath" << "\t\t\t\tSerial number" << endl;
	cout << "------------------------------------------------------------------------------------" << endl;
	for (pair<string, string> p : devices) {
		cout << index << "\t" << p.first << "\t\t" << p.second << endl;
		cout << "------------------------------------------------------------------------------------" << endl;
		index++;
	}
	//I think these strings should be passed to the GUI with a socket, to let the user choose the correct SEcube.
	//In this way the GUI will generate a number that can be passed to the login function.
	return numdevices;
}

// List all the stored keys inside the SEcube device
// returns: number of stored keys inside the SEcube device, or -1 in case of error.
int list_keys() {

	vector<pair<uint32_t, uint16_t>> keys;
	try{
		l1->L1KeyList(keys);
	} catch (...) {
		cout << "Unexpected error trying to list the stored keys..." << endl;
		return -1;
	}

	cout << "Keys stored inside the SEcube device:" << endl;
	if(keys.size() == 0){
		cout << "\nThere are no keys currently stored inside the SEcube device." << endl;
	} else {
		int cnt = 0;
		for(pair<uint32_t, uint16_t> k : keys){
			cout << cnt << ") Key ID " << k.first << " - length: " << 8*k.second << " bit" << endl;
			cnt++;
		}
	}

	return keys.size();
}

int login(array<uint8_t, L1Parameters::Size::PIN> pin, int device) {

	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
	if (ret) {
		cerr << "\nError while searching for SEcube devices! Quit." << endl;
		return -1;
	}

	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
	if (numdevices == 0) {
		cerr << "\nNo SEcube devices found! Quit." << endl;
		return -1;
	}

	if ((device >= 0) && (device < numdevices)) {
		std::array<uint8_t, L0Communication::Size::SERIAL> sn = { 0 };
		if (devices.at(device).second.length() > L0Communication::Size::SERIAL) {
			cerr << "Unexpected error...quit." << endl;
			return -1;
		} else {
			memcpy(sn.data(), devices.at(device).second.data(),
					devices.at(device).second.length());
		}
		l1->L1SelectSEcube(sn);
		cout << "Selected device:" << devices.at(device).first << " - "
				<< devices.at(device).second << endl;

		try {
			l1->L1Login(pin, SE3_ACCESS_USER, true);

		} catch (...) { // catch any kind of exception (login will throw if the password is wrong or if any error happens)
			cerr << "SEcube login error. Check the pin and retry." << endl;
			return -1;
		}

		if (!l1->L1GetSessionLoggedIn()) { // check if login was ok
			cerr << "SEcube login error. Quit." << endl;
			return -1;
		} else {
			cout << "SEcube login OK" << endl;
		}

	}
	return 0;
}

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

void print_command_line() {
	cout
			<< "************************************************************************"
			<< endl;
	cout << "SECube Utilities: " << endl;
	cout << "SCU [-help] [-dev <deviceID>] [-p <pin>] [-e|-d|-di|-dl|-kl] [-f <filename path] [-k <keyID>] [-aes|-sha|-hmac]" << endl;
	cout << "\t-p <pin>" << endl;
	cout << "\t-e encryption" << endl;
	cout << "\t-d decryption" << endl;
	cout << "\t-di digest" << endl;
	cout << "\t-dl devices list" << endl;
	cout << "\t-kl keys list" << endl;
	cout << "\t-f <filename path>: filename path to use for the selected utility"
			<< endl;
	cout << "\t-k <keyID>: key ID to use for encrypt or compute digest" << endl;
	cout << "\t-aes AES_HMACSHA256 (encryption only)" << endl;
	cout << "\t-sha SHA-256 (digest only, no key required)" << endl;
	cout << "\t-hmac HMAC-SHA-256 (digest only)" << endl;
	cout
			<< "************************************************************************"
			<< endl;
}
