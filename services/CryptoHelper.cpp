#include "CryptoHelper.h"
#include <botan/bcrypt.h>
#include <botan/system_rng.h>

std::string CryptoHelper::hashPassword(const std::string& password) {
    Botan::System_RNG rng;
    return Botan::generate_bcrypt(password, rng, 12);
}
