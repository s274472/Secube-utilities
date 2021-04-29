//#include "../sources/L1/L1.h"
//#include <thread> // sleep_for
//#include <fstream>
//#define DEBUG
//
//using namespace std;
//
//unique_ptr<L0> l0 = make_unique<L0>();
//unique_ptr<L1> l1 = make_unique<L1>();
//
////algorithms : 0) SHA-256 1) HMAC-SHA-256
//
//int digest_from_file(array<uint8_t, 32> pin, int algo_number, char *filename) {
//
//	cout << "Looking for SEcube devices...\n" << endl;
//	this_thread::sleep_for(chrono::milliseconds(1000));
//
//	//Get the string from file, to compute digest
//	ifstream fileP(filename, ios::binary|ios::in|ios::ate);
//	if (fileP.is_open()) {
//		streampos size;  //pointer to a point in the streambuf
//		char * memblock;
//		size = fileP.tellg();
//		memblock = new char [size];
//		memset(memblock, '\0', size);
//		encfile.seekg (0, ios::beg);
//		encfile.read (memblock, size);
//		encfile.close();
//		string digest_input(memblock, size);
//		delete[] memblock;
//	}
//	else {
//		cout << "\nError opening file. Quit\n" << endl;
//		return -1;
//	}
//
//	//Now the file to compute digest is in RAM in variable "digest_input".
//	//Let's calculate the length
//	int testsize = strlen(digest_input);
//
//	//Time to contact the SEcube
//	int numdevices = l0->GetNumberDevices(); // this API checks how many SEcube devices are connected to the PC
//	if(numdevices == 0){
//		cout << "No SEcube devices found! Quit." << endl;
//		return 0;
//	}
//	vector<pair<string, string>> devices;
//	int ret = l0->GetDeviceList(devices); // this API fills the vector with pairs including details about the devices (path and serial number)
//	if(ret){
//		cout << "Error while searching for SEcube devices! Quit." << endl;
//		return -1;
//	}
//	cout << "Number of SEcube devices found: " << numdevices << endl;
//	cout << "List of SEcube devices (path, serial number):" << endl;
//	int index = 0;
//	uint8_t empty_serial_number[L0Communication::Size::SERIAL];
//	memset(empty_serial_number, 0, L0Communication::Size::SERIAL);
//	for(pair<string, string> p : devices){
//		if(p.second.empty() || memcmp(p.second.data(), empty_serial_number, L0Communication::Size::SERIAL)==0){
//			cout << index << ") " << p.first << " - serial number not available (please initialize this SEcube)" << endl;
//		} else {
//			cout << index << ") " << p.first << " - " << p.second << endl;
//		}
//		index++;
//	}
//
//	int sel = 0;
//	cout << "\nEnter the number corresponding to the SEcube device that you want to use..." << endl;
//	/* warning: if cin does not wait for input in debug mode with eclipse, open the launch configuration and select
//	 * the "use external console for inferior" checkbox under the debugger tab (see https://stackoverflow.com/questions/44283534/c-debug-mode-in-eclipse-causes-program-to-not-wait-for-cin)*/
//	if(!(cin >> sel)){
//		cout << "Input error...quit." << endl;
//		return -1;
//	}
//
//	// for later usage
//	vector<pair<uint32_t, uint16_t>> keys;
//	int cnt = 0, ch = 0;
//
//	if((sel >= 0) && (sel < numdevices)){
//		array<uint8_t, L0Communication::Size::SERIAL> sn = {0};
//		if(devices.at(sel).second.length() > L0Communication::Size::SERIAL){
//			cout << "Unexpected error...quit." << endl;
//			return -1;
//		} else {
//			memcpy(sn.data(), devices.at(sel).second.data(), devices.at(sel).second.length());
//		}
//		l1->L1SelectSEcube(sn); // select secube with correct serial number
//		cout << "\nDevice " << devices.at(sel).first << " - " << devices.at(sel).second << " selected." << endl;
//    //Then, let's login
//		l1->L1Login(pin, SE3_ACCESS_USER, true); // login to the SEcube
//
//	//Debug
//		#IFDEF DEBUG
//		cout << "\nWe are going to compute the digest of the following string:" << endl;
//		cout << digest_input << endl;
//		#ENDIF
//
//	//Time to do the digest
//		shared_ptr<uint8_t[]> input_data(new uint8_t[testsize]); // to be sent to digest API
//		memcpy(input_data.get(), digest_input, testsize);
//
//
//		cout << "\nStarting digest computation..." << endl;
//
//		SEcube_digest data_digest;
//		SEcube_digest temp; // this is used to verify the digest in case of HMAC-SHA-256 recomputing the digest using the nonce set by the previous computation
//		switch(algo_number){
//			case 0:
//				// when using SHA-256, you don't need to set anything else than the algorithm
//				data_digest.algorithm = L1Algorithms::Algorithms::SHA256;
//				l1->L1Digest(testsize, input_data, data_digest);
//				break;
//			case 1:
//				/* when using HMAC-SHA-256, we also need to provide other details. this type of digest is
//				 * authenticated by means of a shared secret (i.e. a symmetric key), therefore we must provide
//				 * the ID of the key to be used for authentication. we also need to set the value of the usenonce
//				 * flag to false or true. this value should always be false, unless you want to compute the digest
//				 * using a specific nonce to begin with, which is useful for instance if you already have the value
//				 * of the digest computed on the same data with the same algorithm, and you want to recompute it
//				 * (therefore using the same nonce you used before) to see if the digest is still the same or not. */
//				cout << "\nThese are the keys stored on the SEcube." << endl;
//				cout << "Please enter the number of the key that you would like to use for computing the authenticated digest." << endl;
//				cout << "Notice that, if you choose a key that is not compatible with the algorithm you chose at the previous step, the digest computation will fail." << endl;
//				try{
//					l1->L1KeyList(keys);
//				} catch (...) {
//					cout << "Error retrieving keys inside the SEcube device. Quit." << endl;
//					l1->L1Logout();
//					return -1;
//				}
//				if(keys.size() == 0){
//					cout << "There are no keys inside the SEcube device. Impossible to continue." << endl;
//					l1->L1Logout();
//					return -1;
//				}
//				for(pair<uint32_t, uint16_t> k : keys){
//					cout << cnt << ") Key ID " << k.first << " - length: " << 8*k.second << " bit" << endl;
//					cnt++;
//				}
//				if(!(cin >> ch)){
//					cout << "Input error...quit." << endl;
//					l1->L1Logout();
//					return -1;
//				}
//				data_digest.key_id = keys.at(ch).first; // use the selected key ID
//				data_digest.usenonce = false; // we don't want to provide a specific nonce manually
//				data_digest.algorithm = L1Algorithms::Algorithms::HMACSHA256;
//				l1->L1Digest(testsize, input_data, data_digest);
//				// this is used to verify the digest in case of HMAC-SHA-256 recomputing the digest using the nonce set by the previous computation
//				temp.key_id = keys.at(ch).first;
//				temp.usenonce = true;
//				temp.algorithm = L1Algorithms::Algorithms::HMACSHA256;
//				temp.digest_nonce = data_digest.digest_nonce;
//				l1->L1Digest(testsize, input_data, temp);
//				break;
//			// notice that, after calling the L1Digest() function, our digest will be stored inside the Digest object
//			default:
//				cout << "Input error...quit." << endl;
//				l1->L1Logout();
//				return -1;
//		}
//
//		this_thread::sleep_for(chrono::milliseconds(1000));
//		cout << "\n\nThe hex value of the digest is:" << endl;
//		for(uint8_t i : data_digest.digest){
//			printf("%02x ", i);
//		}
//
//		// print also recomputed digest (if any)
//		if(data_digest.algorithm == L1Algorithms::Algorithms::HMACSHA256){
//			cout << "\n\nThe hex value of the recomputed digest is:" << endl;
//			for(uint8_t i : temp.digest){
//				printf("%02x ", i);
//			}
//			if(temp.digest == data_digest.digest){
//				cout << "\nOriginal digest and recomputed digest are equal. OK." << endl;
//			} else {
//				cout << "\nOriginal digest and recomputed digest are not equal. Something went wrong..." << endl;
//			}
//		}
//
//		l1->L1Logout();
//		cout << "\nExample completed. Press 'q' to quit." << endl;
//		while(cin.get() != 'q'){};
//	} else {
//		cout << "You entered an invalid number. Quit." << endl;
//		return 0;
//	}
//	return 0;
//}
