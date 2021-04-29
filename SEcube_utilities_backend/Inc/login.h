#ifndef LOGIN_H_
#define LOGIN_H_
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

using namespace std;

int login(array<uint8_t, L1Parameters::Size::PIN>);

#endif
