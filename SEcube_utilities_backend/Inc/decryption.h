#ifndef DECRYPTION_H
#define DECRYPTION_H
#include "../sefile/SEfile.h"
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>

using namespace std;

#define BUFF_SIZE 1048576

int decryption(string);

#endif
