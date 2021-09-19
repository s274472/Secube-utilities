#include "../Inc/digest.h"
extern unique_ptr<L1> l1;


//algorithms : 0) SHA-256 (no key required) 1) HMAC-SHA-256

int digest(int sock, string filename, uint32_t keyID, string algo) {

	Response_GENERIC resp; // Response to GUI, used if gui_server_on

	int algo_number;

	cout << "SEcube digest utility" << endl << endl;

	if(algo.compare("SHA-256") == 0) {
		algo_number = 0;
	}
	else if (algo.compare("HMAC-SHA-256") == 0) {
		algo_number = 1;
	}
	else {
		cout << "Invalid algorithm. Quit." << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Invalid encryption algorithm!");
		}

		return -1;
	}

	char* digest_input;
	vector<pair<uint32_t, uint16_t>> keys;
	streampos size;		//pointer to a point in the streambuf
	//Get the string from file, to compute digest
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
		cout << "\nError opening file. Quit\n" << endl;

		// For GUI interfacing:
		if(gui_server_on) {
			sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Error opening file!");
		}

		return -1;
	}

	//Now the file to compute digest is in RAM in variable "digest_input".
	//Let's calculate the length
	int testsize = size;

	//Debug
	#ifdef DEBUG
	cout << "\nWe are going to compute the digest of the following string:" << endl;
	cout << digest_input << endl;
	#endif

	//Time to do the digest
	shared_ptr<uint8_t[]> input_data(new uint8_t[testsize]); // to be sent to digest API
	memcpy(input_data.get(), digest_input, testsize);


	cout << "\nStarting digest computation..." << endl;
	SEcube_digest data_digest;
	SEcube_digest temp; // this is used to verify the digest in case of HMAC-SHA-256 recomputing the digest using the nonce set by the previous computation
	switch(algo_number){
		case 0:
			// when using SHA-256, you don't need to set anything else than the algorithm
			data_digest.algorithm = L1Algorithms::Algorithms::SHA256;
			l1->L1Digest(testsize, input_data, data_digest);
			break;
		case 1:
			/* when using HMAC-SHA-256, we also need to provide other details. this type of digest is
			 * authenticated by means of a shared secret (i.e. a symmetric key), therefore we must provide
			 * the ID of the key to be used for authentication. we also need to set the value of the usenonce
			 * flag to false or true. this value should always be false, unless you want to compute the digest
			 * using a specific nonce to begin with, which is useful for instance if you already have the value
			 * of the digest computed on the same data with the same algorithm, and you want to recompute it
			 * (therefore using the same nonce you used before) to see if the digest is still the same or not. */
			data_digest.key_id = keyID; // use the selected key ID
			data_digest.usenonce = false; // we don't want to provide a specific nonce manually
			data_digest.algorithm = L1Algorithms::Algorithms::HMACSHA256;
			l1->L1Digest(testsize, input_data, data_digest);

			// This code was not elimiated for checking how to set the nonce:
//			temp.key_id = keyID;
//			temp.usenonce = true;
//			temp.algorithm = L1Algorithms::Algorithms::HMACSHA256;
//			temp.digest_nonce = data_digest.digest_nonce;
//			l1->L1Digest(testsize, input_data, temp);
			break;
		// notice that, after calling the L1Digest() function, our digest will be stored inside the Digest object
		default:
			cout << "Input error...quit." << endl;

			// For GUI interfacing:
			if(gui_server_on) {
				sendErrorToGUI<Response_GENERIC>(sock, resp, -1, "Input error!");
			}

			l1->L1Logout();
			return -1;
	}

	this_thread::sleep_for(chrono::milliseconds(1000));

	string digest; // String that will store the digest in hex format
	string nonce; // String that will store the nonce in hex format
	char tmp[4];
	string out_msg; // Message to be sent to the GUI

	// Extract digest string in hex format:
	for(uint8_t i : data_digest.digest){
		sprintf(tmp, "%02x ", i);
		digest += tmp;
	}

	cout << "\nThe hex value of the digest is: " << digest << endl;
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
