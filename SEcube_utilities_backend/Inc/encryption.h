#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_
#include "../sefile/SEfile.h"
#include "../sqlite/sqlite3.h"
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>

using namespace std;

int encryption( std::string , uint32_t , std::string );

#endif /* ENCRYPTION_H_ */
