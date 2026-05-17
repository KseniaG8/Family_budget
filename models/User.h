#pragma once
#include <string>

struct User {
    int id;
    std::string login;
    std::string password;
    std::string totp_secret; 
    int is_2fa_enabled;      
    int family_id;           
};
