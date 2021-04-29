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
		l1->L1Logout();
		return -1;
	}

	// Check that the provided keyID is valid:
	vector<pair<uint32_t, uint16_t>> keys;
	try{
		l1->L1KeyList(keys);
	} catch (...) {
		cout << "\nError retrieving keys inside the SEcube device. Quit." << endl;
		l1->L1Logout();
		return -1;
	}
	if(keys.size() == 0){
		cout << "\nError, there are no keys inside the SEcube device. Impossible to continue." << endl;
		l1->L1Logout();
		return -1;
	}

	bool contained = false;
	for(pair<uint32_t, uint16_t> k : keys){

		if( k.first == keyID ) { contained = true; break; }
	}
	if( !contained ) {

		cout << "\nError, the provided keyID is not valid. Impossible to continue." << endl;
		l1->L1Logout();
		return -1;
	}

	// Encrypt the desired file using sefile:
	cout << endl << "File to encrypt: " << filename << endl << "KeyID to use for encrypting: " << keyID << endl << "Encryption algorithm: " << encAlgo << endl;

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
		cout << "Unable to open file " << filename << endl;
//		l1->L1Logout();
		return -1;
	}
	while (getline (inFile, currLine)) {

		cout << currLine << endl;
		currLine = currLine + "\n";
		file1.secure_write((uint8_t*)currLine.c_str(), currLine.size());
	}

	// Close the file:
	inFile.close();
	file1.secure_close();

	SEfile file2(l1.get()); // we create another SEfile object (we could have used the previous one)
	file2.secure_open((char*)filename.c_str(), SEFILE_READ, SEFILE_OPEN); // notice that we simply read the file and we open it because it already exists
	file2.secure_seek(0, &pos, SEFILE_BEGIN); // not really necessary, when a file is opened its pointer is already set to the first sector (header excluded)
	uint32_t filedim;
	secure_getfilesize((char*)filename.c_str(), &filedim, l1.get()); // here we retrieve the total size of the valid data stored inside the file
	unique_ptr<char[]> filecontent = make_unique<char[]>(filedim);
	unsigned int bytesread;
	file2.secure_read((uint8_t*)filecontent.get(), filedim, &bytesread); // here we simply read the entire file into our buffer
	file2.secure_close(); // we can close our file

	cout << "\nThe encrypted text file has been created successfully.\nHere is the content of the encrypted file, as if it was read by "
			"any text editor that cannot use the SEfile APIs to decrypt the content:" << endl;
	char encoded_filename[256]; // we generate the encoded filename and read the encrypted file as a normal file just to show that it is encrypted...this is not required in real applications
	uint16_t enclen = 0;
	memset(encoded_filename, '\0', 256);
	uint16_t r = crypto_filename((char*)filename.c_str(), encoded_filename, &enclen); // generate the name of the encrypted file starting from the original name (encoded filename = SHA-256 of original name)
	if(r){
		cout << "\nUnexpected error. Quit." << endl;
//		l1->L1Logout();
		return -1;
	}
	ifstream encfile(encoded_filename, ios::binary|ios::in|ios::ate);
	if(encfile.is_open()){
		streampos size;
		char * memblock;
		size = encfile.tellg();
	    memblock = new char [size];
	    memset(memblock, '\0', size);
	    encfile.seekg (0, ios::beg);
	    encfile.read (memblock, size);
	    encfile.close();
	    string str(memblock, size);
	    delete[] memblock;
	    cout << str << endl;
	} else {
		cout << "\nUnexpected error. Quit." << endl;
//		l1->L1Logout();
		return -1;
	}

	cout << "\nAs you can see, the content is not the same that we showed at the beginning.\nThis is because the original data (plaintext) is now "
			"encrypted (ciphertext), its real value can be read only using the SEfile APIs, the SEcube and the correct key for decrypting "
			"the data.\nAs a matter of proof, here is the decrypted content of the file, read by the SEfile APIs:" << endl;
	string readcontent(filecontent.get(), filedim);
	cout << readcontent << endl; // we print the content of the text file

	return 0;
}


