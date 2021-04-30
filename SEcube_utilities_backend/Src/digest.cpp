#include "../Inc/digest.h"

extern unique_ptr<L1> l1;


//algorithms : 0) SHA-256 (no key required) 1) HMAC-SHA-256

int digest(string filename, uint32_t keyID, string algo) {

	int algo_number;

	if(algo.compare("SHA-256") == 0) {
		algo_number = 0;
	}
	else if (algo.compare("HMAC-SHA-256") == 0) {
		algo_number = 1;
	}
	else {
		cout << "Invalid algorithm. Quit." << endl;
	}

	char* digest_input;
	vector<pair<uint32_t, uint16_t>> keys;

	//Get the string from file, to compute digest
	ifstream fileP(filename, ios::binary|ios::in|ios::ate);
	if (fileP.is_open()) {
		streampos size;  //pointer to a point in the streambuf
		char * memblock;
		size = fileP.tellg();
		memblock = new char [size];
		memset(memblock, '\0', size);
		fileP.seekg (0, ios::beg);
		fileP.read (memblock, size);
		fileP.close();
		digest_input = memblock;
	}
	else {
		cout << "\nError opening file. Quit\n" << endl;
		return -1;
	}

	//Now the file to compute digest is in RAM in variable "digest_input".
	//Let's calculate the length
	int testsize = strlen(digest_input);


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
				// this is used to verify the digest in case of HMAC-SHA-256 recomputing the digest using the nonce set by the previous computation
				temp.key_id = keyID;
				temp.usenonce = true;
				temp.algorithm = L1Algorithms::Algorithms::HMACSHA256;
				temp.digest_nonce = data_digest.digest_nonce;
				l1->L1Digest(testsize, input_data, temp);
				break;
			// notice that, after calling the L1Digest() function, our digest will be stored inside the Digest object
			default:
				cout << "Input error...quit." << endl;
				l1->L1Logout();
				return -1;
		}

		this_thread::sleep_for(chrono::milliseconds(1000));
		cout << "\n\nThe hex value of the digest is:" << endl;
		for(uint8_t i : data_digest.digest){
			printf("%02x ", i);
		}

		// print also recomputed digest (if any)
		if(data_digest.algorithm == L1Algorithms::Algorithms::HMACSHA256){
			cout << "\n\nThe hex value of the recomputed digest is:" << endl;
			for(uint8_t i : temp.digest){
				printf("%02x ", i);
			}
			if(temp.digest == data_digest.digest){
				cout << "\nOriginal digest and recomputed digest are equal. OK." << endl;
			} else {
				cout << "\nOriginal digest and recomputed digest are not equal. Something went wrong..." << endl;
			}
		}
		return 0;
}
