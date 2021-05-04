#include "../Inc/decryption.h"

extern unique_ptr<L0> l0;
extern unique_ptr<L1> l1;

int decryption(string filename) {

	cout << "SEcube decrypt utility" << endl << endl;

	SEfile file1(l1.get());
	file1.secure_open((char*) filename.c_str(), SEFILE_READ, SEFILE_OPEN);
	int pos;
	file1.secure_seek(0, &pos, SEFILE_BEGIN); // not really necessary, when a file is opened its pointer is already set to the first sector (header excluded)
	uint32_t filedim;
	secure_getfilesize((char*) filename.c_str(), &filedim, l1.get()); // here we retrieve the total size of the valid data stored inside the file
	unique_ptr<char[]> filecontent = make_unique<char[]>(filedim);
	unsigned int bytesread;
	file1.secure_read((uint8_t*) filecontent.get(), filedim, &bytesread); // here we simply read the entire file into our buffer
	file1.secure_close(); // we can close our file

	string readcontent(filecontent.get(), filedim);
	cout << "\nThe content of the encrypted file is:\n" << readcontent << endl; // we print the content of the text file
	this_thread::sleep_for(chrono::milliseconds(1000));

	return 0;
}
