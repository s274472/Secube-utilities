#ifndef DIGEST_H_
#define DIGEST_H_

#include "../sources/L1/L1.h"
#include <thread> // sleep_for
#include <fstream>

#ifdef __linux__
    #include "../Inc/linux_GUI_interface.h"
#elif _WIN32
	#include "../Inc/GUI_interface.h"
#endif

using namespace std;

int digest(int sock, string filename, uint32_t keyID, string algo, bool usenonce, std::array<uint8_t, B5_SHA256_DIGEST_SIZE> nonce);

#endif /* SECUBE_UTILITIES_SECUBE_UTILITIES_BACKEND_INC_DIGEST_H_ */
