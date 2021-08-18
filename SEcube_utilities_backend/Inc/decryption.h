#ifndef DECRYPTION_H
#define DECRYPTION_H
#include "../sefile/SEfile.h"
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>

using namespace std;

#define BUFF_SIZE 1048576

int decryption(string);
int decryption_w_encrypted_filename(int sock, string filename);

#endif
