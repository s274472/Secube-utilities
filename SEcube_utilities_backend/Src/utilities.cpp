#include "../Inc/utilities.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

//Will return the number of devices found.
int list_devices(int sock) {

	Response_DEV_LIST resp; // Response to GUI, used if gui_server_on

	cout << "Looking for SEcube devices..." << endl;
	this_thread::sleep_for(chrono::milliseconds(2000));

	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
	if (ret) {
		cerr << "\nError while searching for SEcube devices! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_DEV_LIST>(sock, resp, -1, "Error while searching for SEcube devices!");
		}

		return -1;
	}

	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
	if (numdevices == 0) {
		cerr << "\nNo SEcube devices found! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_DEV_LIST>(sock, resp, -1, "No SEcube devices found!");
		}

		return -1;
	}
	cout << "Number of SEcube devices found: " << numdevices << endl;
	int index = 0;
	cout << "------------------------------------------------------------------------------------" << endl;
	cout << "DevID" << "\t\tPath" << "\t\t\t\tSerial number" << endl;
	cout << "------------------------------------------------------------------------------------" << endl;
	for (pair<string, string> p : devices) {
		cout << index << "\t" << p.first << "\t\t" << p.second << endl;
		cout << "------------------------------------------------------------------------------------" << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			// Prepare response to GUI:
			strcpy(resp.paths[index], p.first.c_str());
			strcpy(resp.serials[index], p.second.c_str());
		}

		index++;
	}

	// For GUI interfacing:
	if(gui_server_on) {

		// Prepare response to GUI:
		resp.err_code = 0;
		resp.num_devices=numdevices;
		sendResponseToGUI<Response_DEV_LIST>(sock, resp);
	}


	//I think these strings should be passed to the GUI with a socket, to let the user choose the correct SEcube.
	//In this way the GUI will generate a number that can be passed to the login function.
	return numdevices;
}

// List all the stored keys inside the SEcube device
// returns: number of stored keys inside the SEcube device, or -1 in case of error.
int list_keys(int sock) {

	Response_LIST_KEYS resp; // Response to GUI, used if gui_server_on

	vector<pair<uint32_t, uint16_t>> keys;
	try{
		l1->L1KeyList(keys);
	} catch (...) {
		cout << "Unexpected error trying to list the stored keys..." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_LIST_KEYS>(sock, resp, -1, "Unexpected error trying to list the stored keys...");
		}

		return -1;
	}

	cout << "Keys stored inside the SEcube device:" << endl;
	if(keys.size() == 0){
		cout << "\nThere are no keys currently stored inside the SEcube device." << endl;
	} else {
		int cnt = 0;
		for(pair<uint32_t, uint16_t> k : keys){
			cout << cnt << ") Key ID " << k.first << " - length: " << 8*k.second << " bit" << endl;

			// For GUI interfacing:
			if(gui_server_on) {
				// Prepare response to GUI:
				resp.key_ids[cnt] = k.first;
				resp.key_sizes[cnt] = k.second;
			}

			cnt++;
		}
	}

	// For GUI interfacing:
	if(gui_server_on) {
		// Prepare response to GUI:
		resp.err_code = 0;
		resp.num_keys = keys.size();
		sendResponseToGUI<Response_LIST_KEYS>(sock, resp);
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
int find_key (uint32_t& keyID, string user, string group){
	bool keyfound = false;
	string chosen;
	sekey_error rc;
	if(sekey_start(*l0, l1.get()) != 0){
		cout << "Error starting SEkey!" << endl;
	}
	if (user.length() > 0){
		vector<string> users;
		char U[user.length() + 1];
		strcpy(U, user.c_str());
		sekey_error rc;
		char delim[] = " ";
		char *ptr = strtok(U,delim);
		int i = 0;
		while(ptr != NULL){
			users.push_back(ptr);
			ptr = strtok(NULL, delim);
			i++;
		}
		if (users.size() == 1){
			rc = (sekey_error)sekey_find_key_v1(chosen, users[0], se_key_type::symmetric_data_encryption);
			if (rc == SEKEY_OK) {
				cout << "Key for " + users[0] + ": " + chosen << endl;
				keyfound = true;
			} else {
				cout << "Key for " + users[0] + " not found. Returned value: " << rc << endl;
			}
		} else {
			rc = (sekey_error)sekey_find_key_v3(chosen, users, se_key_type::symmetric_data_encryption);
			if (rc == SEKEY_OK) {
				cout << "Key for " + user + ": " + chosen << endl;
				keyfound = true;
			} else {
				cout << "Key for " + user + " not found. Returned value: " << rc << endl;
			}
		}
	} else if (group.size() != 0){
		rc = (sekey_error)sekey_find_key_v2(chosen, group, se_key_type::symmetric_data_encryption);
			if (rc == SEKEY_OK) {
				cout << "Key for " + group + ": " + chosen << endl;
				keyfound = true;
			} else {
				cout << "Key for " + group + " not found. Returned value: " << rc << endl;
			}
	}
	if (keyfound) {
		string key;
		key = chosen.substr(1,chosen.length()-1);

		keyID = ((uint32_t)stoul(key));
		sekey_stop();
		return 1;
	} else {
		sekey_stop();
		return 0;
	}
}

void print_command_line() {
	cout
			<< "************************************************************************"
			<< endl;
	cout << "SECube Utilities: " << endl;
	cout << "SCU [-help] [-dev <deviceID>] [-p <pin>] [-e|-d|-dk|-di|-dl|-kl] [-update_path <path>] [-u <user(s)>|-g <group>][-f <filename path] [-k <keyID>] [-aes|-sha|-hmac|aes_hmac]" << endl;
	cout << "\t-dev <deviceID>" << endl;
	cout << "\t-p <pin>" << endl;
	cout << "\t-e encryption" << endl;
	cout << "\t-d decryption" << endl;
	cout << "\t-dk decryption using SEkey" << endl;
	cout << "\t-di digest" << endl;
	cout << "\t-dl devices list" << endl;
	cout << "\t-kl keys list" << endl;
	cout << "\t-update_path <SEkey path> updates the path of the shared folder for the SEkey KMS" << endl;
	cout << "\t-u <user(s)> if specified, find keys automatically (put list of users between " ", each one separated by space)" << endl;
	cout << "\t-g <group> if specified, find keys automatically" << endl;
	cout << "\t-f <filename path>: filename path to use for the selected utility"
			<< endl;
	cout << "\t-k <keyID>: key ID to use for encrypt or compute digest" << endl;
	cout << "\t-aes_hmac AES_HMACSHA256 (encryption only)" << endl;
	cout << "\t-sha SHA-256 (digest only, no key required)" << endl;
	cout << "\t-hmac HMAC-SHA-256 (digest only)" << endl;
//	cout << "\t-aes AES" << endl;
	cout
			<< "************************************************************************"
			<< endl;
}
