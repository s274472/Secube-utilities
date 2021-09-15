#ifndef DECRYPTION_H
#define DECRYPTION_H
#include "../sefile/SEfile.h"
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>

#ifdef __linux__
    #include "../Inc/linux_GUI_interface.h"
#elif _WIN32
	#include "../Inc/GUI_interface.h"
#endif

using namespace std;

#define BUFF_SIZE 1048576

int decryption(string);
int decryption_w_encrypted_filename(int sock, string filename);

#endif
