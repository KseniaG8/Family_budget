#pragma once
#include <string>

struct User {
    int id;
    std::string login;
    std::string password;
<<<<<<< HEAD
};
=======
    std::string totp_secret; 
    int is_2fa_enabled;               
};
>>>>>>> 22e5bb2d01344dce828f490fe50a23d68012dcbf
