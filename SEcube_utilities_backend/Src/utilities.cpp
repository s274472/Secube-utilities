/**
 * All the available utilities are implemented here.
 *
 * In case the gui_server flag is set, the output of the called utility is forwarded to the GUI via socket.
 * For more details on how the socket communication interface with the GUI is implemented, please take a look at the
 * GUI_interface.h and GUI_interface.cpp sources (and the related linux implementation: linux_GUI_interface.h and linux_GUI_interface.cpp).
 */

#include "../Inc/utilities.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

/**
 * Computes and prints on console the digest of the specified input file using the specified algorithm and keyID
 * algorithms: 0) SHA-256 (no key required) - 1) HMAC-SHA-256 (key required)
 *
 * If usenonce is true, the provided nonce is used for the digest computation(using the HMAC-SHA-256 algorithm), otherwise the nonce
 * is computed automatically.
 *
 * In order to call this utility, first login on the desired SECube device!
 *
 * returns: 0 if the digest was correctly computed, -1 in case of error
 */
int digest(int sock, string filename, uint32_t keyID, string algo, bool usenonce, std::array<uint8_t, B5_SHA256_DIGEST_SIZE> nonce) {

	Response_GENERIC resp; // Response to GUI, used if gui_server_on

	int algo_number;

	cout << "SEcube digest utility" << endl << endl;

	// Check if the specified algorithm is correct:
	if(algo.compare("SHA-256") == 0) {
		algo_number = 0;
	}
	else if (algo.compare("HMAC-SHA-256") == 0) {
		algo_number = 1;
	}
	else {
		cout << "Invalid algorithm! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Invalid algorithm!");
		}

		return -1;
	}

	char* digest_input;
	vector<pair<uint32_t, uint16_t>> keys;
	streampos size; // pointer to a point in the streambuf

	// Get the string from file, to compute digest:
	ifstream fileP((char*)filename.c_str(), ios::binary|ios::in|ios::ate);
	if (fileP.is_open()) {
		char * memblock;
		size = fileP.tellg();
		memblock = new char [size];
		memset(memblock,'\0', size);
		fileP.seekg (0, ios::beg);
		fileP.read (memblock, size);
		fileP.close();
		digest_input = memblock;
	}
	else {
		cout << "Error opening file! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Error opening file!");
		}

		return -1;
	}

	// Now the file to compute digest is in RAM in variable "digest_input".
	// Let's calculate the length:
	int testsize = size;

	//Time to do the digest:
	shared_ptr<uint8_t[]> input_data(new uint8_t[testsize]); // to be sent to digest API
	memcpy(input_data.get(), digest_input, testsize);


	cout << "Starting digest computation..." << endl;
	SEcube_digest data_digest;
	switch(algo_number){
		case 0: // SHA-256
			// When using SHA-256, you don't need to set anything else than the algorithm
			data_digest.algorithm = L1Algorithms::Algorithms::SHA256;
			l1->L1Digest(testsize, input_data, data_digest);
			break;
		case 1: // HMAC-SHA-256
			/* When using HMAC-SHA-256, we also need to provide other details. this type of digest is
			 * authenticated by means of a shared secret (i.e. a symmetric key), therefore we must provide
			 * the ID of the key to be used for authentication. We also need to set the value of the usenonce
			 * flag to false or true. This value should always be false, unless you want to compute the digest
			 * using a specific nonce to begin with, which is useful for instance if you already have the value
			 * of the digest computed on the same data with the same algorithm, and you want to recompute it
			 * (therefore using the same nonce you used before) to see if the digest is still the same or not. */
			data_digest.key_id = keyID; // use the selected key ID
			data_digest.algorithm = L1Algorithms::Algorithms::HMACSHA256;

			if( !usenonce ) {
				data_digest.usenonce = false; // We don't want to provide a specific nonce manually
			} else {
				data_digest.usenonce = true; //  We want to provide a specific nonce manually
				data_digest.digest_nonce = nonce;
			}

			l1->L1Digest(testsize, input_data, data_digest);

			break;
		// notice that, after calling the L1Digest() function, our digest will be stored inside the Digest object
		default:
			cout << "Input error! Quit." << endl;

			// For GUI interfacing:
			if(gui_server_on) {
				sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Input error!");
			}

			l1->L1Logout();
			return -1;
	}

	string digest; // String that will store the digest in hex format
	string nonce; // String that will store the nonce in hex format
	char tmp[4]; // Used for converting the digest and nonce to the %02x format
	string out_msg; // Message to be sent to the GUI, if gui_server_on

	// Extract digest string in hex format:
	for(uint8_t i : data_digest.digest){
		sprintf(tmp, "%02x ", i);
		digest += tmp;
	}

	cout << "The hex value of the digest is: " << digest << endl;
	out_msg = "digest hex: " + string(digest);

	// If the algorithm selected is the HMAC-SHA-256, the nonce must be extracted:
	if(algo.compare("HMAC-SHA-256") == 0) {

		// Extract nonce string in hex format:
		for(uint8_t i : data_digest.digest_nonce){
			sprintf(tmp, "%02x ", i);
			nonce += tmp;
		}

		cout << "The hex value of the nonce is: " << nonce << endl;
		out_msg += "\n\nnonce hex: " + string(nonce);
	}

	// For GUI interfacing:
	if(gui_server_on) {

		sendErrorToGUI<Response_GENERIC>(sock, resp, 0, out_msg); // In this case err_code = 0 means everything went correctly
	}

	return 0;
}

/**
 * Encrypts the input file using SEFile and stores it in the same path.
 * Due to the current SEFile implementation, only the AES_HMACSHA256 algorithm can be used.
 * In order to call this utility, first login on the desired SECube device!
 *
 * returns: 0 if the encryption is successful, -1 in case of error
 */
int encryption( int sock, string filename, uint32_t keyID, string encAlgo ) {

	Response_GENERIC resp; // Response to GUI, used if gui_server_on

	int encAlgoID;

	// Print the title:
	cout << "SEcube encrypt utility" << endl << endl;

	// Only the AES_HMACSHA256 algorithm can be used for encrypting using SEFile:
	if( encAlgo == "AES_HMACSHA256" ) {

		encAlgoID = L1Algorithms::Algorithms::AES_HMACSHA256;
	} else {

		cout << "Invalid encryption algorithm!" << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Invalid encryption algorithm!");
		}

		return -1;
	}

	// Check that the provided keyID is valid:
//	vector<pair<uint32_t, uint16_t>> keys;
//	try{
//		l1->L1KeyList(keys);
//	} catch (...) {
//		cout << "\nError retrieving keys inside the SEcube device. Quit." << endl;
//		//l1->L1Logout();
//		return -1;
//	}
//	if(keys.size() == 0){
//		cout << "\nError, there are no keys inside the SEcube device. Impossible to continue." << endl;
//		//l1->L1Logout();
//		return -1;
//	}
//
//	bool contained = false;
//	for(pair<uint32_t, uint16_t> k : keys){
//
//		if( k.first == keyID ) { contained = true; break; }
//	}
//	if( !contained ) {
//
//		cout << "\nError, the provided keyID is not valid. Impossible to continue." << endl;
//		//l1->L1Logout();
//		return -1;
//	}

	// Encrypt the desired file using SEFile:
	cout << "File to encrypt: " << filename << endl << "KeyID to use for encrypting: " << keyID << endl << "Encryption algorithm: " << encAlgo << endl;

	SEfile file1(l1.get(), keyID, encAlgoID); // We create a SEfile object bounded to the L1 SEcube object, we specify also the ID of the key and the algorithm that we want to use

	// Open the file to encrypt using sefile secure_open:
	file1.secure_open((char*)filename.c_str(), SEFILE_WRITE, SEFILE_NEWFILE); // If the file already exists it is overwritten
	int pos; /* we move to the end of the file (this is not really necessary here because the file has just been created) */
	file1.secure_seek(0, &pos, SEFILE_END); // append to the end of the file

	// Open the file to encrypt using standard OS system call:
	ifstream inFile(filename, ios::binary);
	char buffer[BUFF_SIZE];

	if( inFile ){

		inFile.seekg(0, std::ios::beg);
		while( inFile.readsome(buffer, BUFF_SIZE) ) {

			file1.secure_write( (uint8_t*)buffer, inFile.gcount() );
		}
	} else {

		cout << "Error encrypting the file! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Error encrypting the file!");
		}

		return -1;
	}

	// Close the files:
	inFile.close();
	file1.secure_close();

	cout << "File correctly encrypted!" << endl;

	// For GUI interfacing:
	if(gui_server_on) {
		sendErrorToGUI<Response_GENERIC>(sock, resp, 0, "File correctly encrypted!"); // In this case err_code = 0 means everything went correctly
	}

	return 0;
}

/**
 * Decrypts the input file using SEFile and stores it in the same path.
 * The input filename must be the decrypted one(to decrypt using the encrypted filename please use: decryption_w_encrypted_filename).
 * In order to call this utility, first login on the desired SECube device!
 *
 * returns: 0 if the decryption is successful, -1 in case of error
 */
int decryption(string filename) {

	// Print the title:
	cout << "SEcube decrypt utility" << endl << endl;
	cout << "File to decrypt: " << filename << endl;

	SEfile file1(l1.get());
	file1.secure_open((char*) filename.c_str(), SEFILE_READ, SEFILE_OPEN);
	int pos;
	file1.secure_seek(0, &pos, SEFILE_BEGIN); // not really necessary, when a file is opened its pointer is already set to the first sector (header excluded)

	// Save the decrypted file:
	ofstream outFile(filename, ios::out|ios::binary);
	char buffer[BUFF_SIZE];
	unsigned int bytesread;

	if( outFile ) {

		do{
			file1.secure_read((uint8_t*) buffer, BUFF_SIZE, &bytesread);
			outFile.write(buffer, bytesread);
		} while( bytesread>0 );
	} else {

		cout << "Error decrypting the file!" << endl;
		return -1;
	}

	// Close files:
	file1.secure_close();
	outFile.close();

	cout << "File correctly decrypted!" << endl;

	return 0;
}

/**
 * Decrypts the input file using SEFile and stores it in the same path.
 * The input filename must be the encrypted one(to decrypt using the decrypted filename please use: decryption).
 * In order to call this utility, first login on the desired SECube device!
 *
 * returns: 0 if the decryption is successful, -1 in case of error
 */
int decryption_w_encrypted_filename(int sock, string filename) {

	Response_GENERIC resp; // Response to GUI, used if gui_server_on

	// Print the title:
	cout << "SEcube decrypt utility" << endl << endl;
	cout << "File to decrypt: " << filename << endl;

	SEfile file1(l1.get());
	vector<pair<string,string>> list;
	secure_ls(filename, list, l1.get());

	int position_of_slash = -1;
	for (int i_filename = filename.length(); i_filename>=0; i_filename--) {
		if (filename[i_filename] == '\\' || filename[i_filename] == '/') {
			position_of_slash = i_filename;
			break;
		}
	}

	if (position_of_slash == -1)
		filename = list[0].second; //because it's relative path
	else {
		filename = filename.erase(position_of_slash + 1, filename.length());
		filename = filename + list[0].second;
	}

	cout << "Will decrypt as:" << endl;
	cout << filename << endl;

	file1.secure_open((char*) filename.c_str(), SEFILE_READ, SEFILE_OPEN);
	int pos;
	file1.secure_seek(0, &pos, SEFILE_BEGIN); // not really necessary, when a file is opened its pointer is already set to the first sector (header excluded)

	// Save the decrypted file:
	ofstream outFile(filename, ios::out|ios::binary);
	char buffer[BUFF_SIZE];
	unsigned int bytesread;

	if( outFile ) {

		do{
			file1.secure_read((uint8_t*) buffer, BUFF_SIZE, &bytesread);
			outFile.write(buffer, bytesread);
		} while( bytesread>0 );
	} else {

		cout << "Error decrypting the file!" << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Error decrypting the file!");
		}

		return -1;
	}

	// Close files:
	file1.secure_close();
	outFile.close();

	cout << "File correctly decrypted!" << endl;

	// For GUI interfacing:
	if(gui_server_on) {
		sendErrorToGUI<Response_GENERIC>(sock, resp, 0, "File correctly decrypted!"); // In this case err_code = 0 means everything went correctly
	}

	return 0;
}

/**
 * Discovers all the SECube devices connected to the PC and prints on console the DeviceID, Path and Serial Number.
 *
 * returns: the number of SECube devices found, -1 in case of error
 */
int list_devices(int sock) {

	Response_DEV_LIST resp; // Response to GUI, used if gui_server_on

	cout << "Looking for SEcube devices..." << endl;

	vector<pair<string, string>> devices; // The elements inside the vector are (path, serial number)
	int ret = l0->GetDeviceList(devices); // This API fills the vector with pairs including details about the devices (path and serial number)
	if (ret) {
		cerr << "Error while searching for SEcube devices! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_DEV_LIST>(sock, resp, -1, "Error while searching for SEcube devices!");
		}

		return -1;
	}

	int numdevices = l0->GetNumberDevices(); // This API checks how many SEcube devices are connected to the PC
	if (numdevices == 0) {
		cerr << "No SEcube devices found! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_DEV_LIST>(sock, resp, -1, "No SEcube devices found!");
		}

		return -1;
	}

	// Print the devices informations on console and prepare the response to the GUI:
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
			// The response will contain: an array of string (paths), for storing the device path
			//							  an array of string (serials), for storing the device serial
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

	return numdevices;
}

/**
 * List all the stored keys inside the SEcube device and prints on console the KeyID and Key Size.
 * In order to call this utility, first login on the desired SECube device!
 *
 * returns: number of stored keys inside the SEcube device, -1 in case of error
 */
int list_keys(int sock) {

	Response_LIST_KEYS resp; // Response to GUI, used if gui_server_on

	vector<pair<uint32_t, uint16_t>> keys;
	try{
		l1->L1KeyList(keys);
	} catch (...) {
		cout << "Unexpected error trying to list the stored keys! Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_LIST_KEYS>(sock, resp, -1, "Unexpected error trying to list the stored keys!");
		}

		return -1;
	}

	// Print the keys informations on console and prepare the response to the GUI:
	cout << "Keys stored inside the SEcube device:" << endl;
	if(keys.size() == 0){
		cout << "There are no keys currently stored inside the SEcube device." << endl;
	} else {
		int cnt = 0;
		for(pair<uint32_t, uint16_t> k : keys){
			cout << cnt << ") Key ID " << k.first << " - length: " << 8*k.second << " bit" << endl;

			// For GUI interfacing:
			if(gui_server_on) {

				// Prepare response to GUI:
				// The response will contain: an array of uint32_t (key_ids), for storing the KeyID
				//							  an array of uint16_t (key_sizes), for storing the Key Size
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

/**
 * Login to the specified SECube  DeviceID using the provided PIN.
 *
 * returns: 0 if the login is successful, -1 in case of error
 */
int login(array<uint8_t, L1Parameters::Size::PIN> pin, int device) {

	vector<pair<string, string>> devices;
	int ret = l0->GetDeviceList(devices); // This API fills the vector with pairs including details about the devices (path and serial number)
	if (ret) {
		cerr << "Error while searching for SEcube devices! Quit." << endl;
		return -1;
	}

	int numdevices = l0->GetNumberDevices(); // This API checks how many SEcube devices are connected to the PC
	if (numdevices == 0) {
		cout << "No SEcube devices found! Quit." << endl;
		return -1;
	}

	if ((device >= 0) && (device < numdevices)) {
		std::array<uint8_t, L0Communication::Size::SERIAL> sn = { 0 };
		if (devices.at(device).second.length() > L0Communication::Size::SERIAL) {
			cout << "Unexpected error! Quit." << endl;
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

		} catch (...) { // Catch any kind of exception (login will throw if the password is wrong or if any error happens)
			cout << "SEcube login error! Check the pin and retry! Quit." << endl;
			return -1;
		}

		if (!l1->L1GetSessionLoggedIn()) { // check if login was ok
			cout << "SEcube login error! Quit." << endl;
			return -1;
		} else {
			cout << "SEcube login OK!" << endl;
		}

	}
	return 0;
}

/**
 * Logout from the previously logged-in SECube device.
 *
 * returns: 0 if the logout is successful, -1 in case of error
 */
int logout() {

	cout << "Logging out..." << endl;
	try {
		l1->L1Logout();
	} catch (...) {
		cout << "Logout error! Quit." << endl;
		return -1;
	}
	if (l1->L1GetSessionLoggedIn()) { // Check if logout was ok
		cout << "Logout error! Quit." << endl;
		return -1;
	}
	cout << "You are now logged out!" << endl;

	return 0;
}

/**
 * Finds a key using SEKey that can be used for the specified users, or group.
 *
 * returns: 1 if a key was found, 0 otherwise
 */
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

/**
 * Prints the helper on the console.
 *
 * returns: void
 */
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
	cout << "\t-nonce \"nonce string\" (if specified, the nonce for the HMAC-SHA-256 is set manually)" << endl;
	cout << "\t-gui_server (should only be specified by the GUI!)" << endl;
	cout
			<< "************************************************************************"
			<< endl;
}
