#include "../Inc/decryption.h"
#include "../Inc/GUI_interface.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

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

int decryption_w_encrypted_filename(int sock, string filename) {

	Response_GENERIC resp;

	// Print the title:
	cout << "SEcube decrypt utility" << endl << endl;
	cout << "File to decrypt: " << filename << endl;

	SEfile file1(l1.get());
	vector<pair<string,string>> list;
	secure_ls(filename, list, l1.get());
	//cout << list[0].first << endl;
	//cout << list[0].second << endl;

	filename = list[0].second;

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
