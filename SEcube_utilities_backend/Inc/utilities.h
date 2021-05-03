#ifndef UTILITIES_H_
#define UTILITIES_H_
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

using namespace std;

int login(array<uint8_t, L1Parameters::Size::PIN>);
int logout();
int list_devices();  //return value: number of found devices, or -1 in case of error.
//list keys
#endif
