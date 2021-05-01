#ifndef UTILITIES_H_
#define UTILITIES_H_
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

using namespace std;

int login(array<uint8_t, L1Parameters::Size::PIN>);
int logout();
//list devices
//list keys
#endif
