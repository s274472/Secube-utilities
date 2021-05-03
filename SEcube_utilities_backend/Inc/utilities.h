#ifndef UTILITIES_H_
#define UTILITIES_H_
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

using namespace std;

#define ENCRYPTION 0
#define DECRYPTION 1
#define DIGEST 2
typedef int utility;
int login(array<uint8_t, L1Parameters::Size::PIN>, int);
int logout();
int parse_args(int argc, char *argv[], char *pin, utility *utility, char *path,
		uint32_t *keyID, string *alg);
void print_command_line();
int list_devices();  //return value: number of found devices, or -1 in case of error.
int list_keys(); // returns: number of stored keys inside the SEcube device, or -1 in case of error.
#endif
