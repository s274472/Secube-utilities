#ifndef UTILITIES_H_
#define UTILITIES_H_
#include "../sefile/environment.h"
#include "../sekey/SEkey.h"

#ifdef __linux__
    #include "../Inc/linux_GUI_interface.h"
#elif _WIN32
	#include "../Inc/GUI_interface.h"
#endif

#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <cmath>
#include <stdlib.h>

using namespace std;

#define BUFF_SIZE 1048576 // Used in encryption and decryption utilities

enum Utilities {DEFAULT, ENCRYPTION, DECRYPTION, DIGEST, DEV_LIST, K_LIST, UPDATE_PATH};

// Utilities:
int login(array<uint8_t, L1Parameters::Size::PIN>, int);
int logout();

void print_command_line(); // Prints the helper on the command_line.

int list_devices(int sock);  // returns: the number of SECube devices found, -1 in case of error
int list_keys(int sock); // returns: number of stored keys inside the SEcube device, -1 in case of error
int find_key(uint32_t& keyID, string user, string group); // returns: 1 if a key was found, 0 otherwise

int digest(int sock, string filename, uint32_t keyID, string algo, bool usenonce, std::array<uint8_t, B5_SHA256_DIGEST_SIZE> nonce); // returns: 0 if the digest was correctly computed, -1 in case of error
int decryption(string); // returns: 0 if the decryption is successful, -1 in case of error
int decryption_w_encrypted_filename(int sock, string filename); // returns: 0 if the decryption is successful, -1 in case of error
int encryption(int sock, string , uint32_t , string); // returns: 0 if the encryption is successful, -1 in case of error
int isKeyContained(int sock, uint32_t keyID); // returns: 1 if the key is contained inside the SECube device, 0 otherwise

#endif
