#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_
#include "../sefile/SEfile.h"
#include "../sqlite/sqlite3.h"
#include "../sefile/environment.h"
#include <thread> // thread::sleep_for
#include <fstream>
#include <iostream>
#include "../Inc/GUI_interface.h"

using namespace std;

#define BUFF_SIZE 1048576

int encryption( int sock, std::string , uint32_t , std::string );

#endif /* ENCRYPTION_H_ */
