#ifndef DIGEST_H_
#define DIGEST_H_

#include "../sources/L1/L1.h"
#include <thread> // sleep_for
#include <fstream>
#include "../Inc/GUI_interface.h"
#define DEBUG

using namespace std;

int digest(int sock, string filename, uint32_t keyID, string algo);

#endif /* SECUBE_UTILITIES_SECUBE_UTILITIES_BACKEND_INC_DIGEST_H_ */
