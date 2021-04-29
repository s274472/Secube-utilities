#include "../Inc/login.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

int login(array<uint8_t, L1Parameters::Size::PIN> pin) {
	int dev_err;
	this_thread::sleep_for(chrono::milliseconds(1000));
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
		return -1;
	}
	cout << "Number of SEcube devices found: " << numdevices << endl;
	cout << "List of SEcube devices (path - serial number):" << endl;
	int index = 0;
	for (pair<string, string> p : devices) {
		cout << index << ") " << p.first << " - " << p.second << endl;
		index++;
	}

	int sel = 0;
	do {
		dev_err = 0;
		this_thread::sleep_for(chrono::milliseconds(1000));
		cout
				<< "\nEnter the number corresponding to the SEcube device you want to use..."
				<< endl;
		if (!(cin >> sel)) {
			cerr << "Input error...quit." << endl;
			return -1;
		} else if (sel < 0 || sel >= numdevices) {
			cerr << "Invalid device number. Please retry." << endl;
			dev_err = 1;
		}
	} while (dev_err != 0);

	if ((sel >= 0) && (sel < numdevices)) {
		std::array<uint8_t, L0Communication::Size::SERIAL> sn = { 0 };
		if (devices.at(sel).second.length() > L0Communication::Size::SERIAL) {
			cerr << "Unexpected error...quit." << endl;
			return -1;
		} else {
			memcpy(sn.data(), devices.at(sel).second.data(),
					devices.at(sel).second.length());
		}
		l1->L1SelectSEcube(sn);
		cout << "Selected device:" << devices.at(sel).first << " - "
				<< devices.at(sel).second << endl;

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
