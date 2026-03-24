#pragma once
#include <string>

class CryptoHelper {
public:
    static std::string hashPassword(const std::string& password);
};
