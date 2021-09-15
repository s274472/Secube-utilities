#ifndef UTILITIES_H_
#define UTILITIES_H_
#include "../sefile/environment.h"
#include "../sekey/SEkey.h"

#ifdef __linux__
    #include "../Inc/linux_GUI_interface.h"
#elif _WIN32
	#include "../Inc/GUI_interface.h"
#endif

#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <cmath>
#include <stdlib.h>
using namespace std;

#define ENCRYPTION 0
#define DECRYPTION 1
#define DIGEST 2
#define DEV_LIST 3
#define K_LIST 4
#define UPDATE_PATH 5
typedef int utility;
int login(array<uint8_t, L1Parameters::Size::PIN>, int);
int logout();
void print_command_line();
int list_devices(int sock);  //return value: number of found devices, or -1 in case of error.
int list_keys(int sock); // returns: number of stored keys inside the SEcube device, or -1 in case of error.
int find_key(uint32_t& keyID, string user, string group);
#endif
