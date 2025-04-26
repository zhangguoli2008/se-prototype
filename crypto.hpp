#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>

namespace Crypto {
std::string encrypt(const std::string& plaintext);
std::string decrypt(const std::string& ciphertext);
}

#endif
