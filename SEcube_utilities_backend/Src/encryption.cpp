#include "../Inc/encryption.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

int encryption( string filename, uint32_t keyID, string encAlgo ) {

	int encAlgoID;

	// Print the title:
	cout << "Sefile encrypt utility" << endl << endl;

	// Check that the provided encAlgo is valid:
	if( encAlgo == "AES") {

		encAlgoID = L1Algorithms::Algorithms::AES;
	} else if( encAlgo == "SHA256" ) {

		encAlgoID = L1Algorithms::Algorithms::SHA256;
	} else if( encAlgo == "HMACSHA256" ) {

		encAlgoID = L1Algorithms::Algorithms::HMACSHA256;
	} else if( encAlgo == "AES_HMACSHA256" ) {

		encAlgoID = L1Algorithms::Algorithms::AES_HMACSHA256;
	} else {

		cout << "Invalid encryption algorithm!" << endl;
		//l1->L1Logout();
		return -1;
	}

	// Check that the provided keyID is valid:
	vector<pair<uint32_t, uint16_t>> keys;
	try{
		l1->L1KeyList(keys);
	} catch (...) {
		cout << "\nError retrieving keys inside the SEcube device. Quit." << endl;
		//l1->L1Logout();
		return -1;
	}
	if(keys.size() == 0){
		cout << "\nError, there are no keys inside the SEcube device. Impossible to continue." << endl;
		//l1->L1Logout();
		return -1;
	}

	bool contained = false;
	for(pair<uint32_t, uint16_t> k : keys){

		if( k.first == keyID ) { contained = true; break; }
	}
	if( !contained ) {

		cout << "\nError, the provided keyID is not valid. Impossible to continue." << endl;
		//l1->L1Logout();
		return -1;
	}

	// Encrypt the desired file using sefile:
	cout << "File to encrypt: " << filename << endl << "KeyID to use for encrypting: " << keyID << endl << "Encryption algorithm: " << encAlgo << endl;

	SEfile file1(l1.get(), keyID, encAlgoID); // we create a SEfile object bounded to the L1 SEcube object, we specify also the ID of the key and the algorithm that we want to use

	// Open the file to encrypt using sefile secure_open:
	file1.secure_open((char*)filename.c_str(), SEFILE_WRITE, SEFILE_NEWFILE); // If the file already exists it is overwritten
	int pos; /* we move to the end of the file (this is not really necessary here because the file has just been created) */
	file1.secure_seek(0, &pos, SEFILE_END); // append to the end of the file

	// Open the file to encrypt using standard OS system call:
	ifstream inFile;
	string currLine;

	inFile.open(filename);
	if (!inFile) {
		cout << "Unable to open file " << filename << "!" << endl;
		// l1->L1Logout();
		return -1;
	}
	while (getline (inFile, currLine)) {

		//cout << currLine << endl;
		currLine = currLine + "\n";
		file1.secure_write((uint8_t*)currLine.c_str(), currLine.size());
	}

	// Close the file:
	inFile.close();
	file1.secure_close();

	cout << "File correctly encrypted!" << endl;

	return 0;
}


