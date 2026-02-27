#include "database/Database.h"
#include "services/TransactionService.h"
#include "handlers/TransactionHandler.h"
#include "services/UserService.h"
#include "handlers/UserHandler.h"
#include <iostream>

int main() {
    Database db("budget.db");
    db.init();
    db.clearTransactions();

    UserService userService(db);
    UserHandler userHandler(userService);

    auto reg = userHandler.registerUser("valeria", "1234");
    std::cout << reg.dump(4) << std::endl;

    auto login = userHandler.loginUser("valeria", "1234");
    std::cout << login.dump(4) << std::endl;
    
    TransactionService service(db);
    TransactionHandler handler(service);

    auto result = handler.getTransactions(1);
    std::cout << result.dump(4) << std::endl;
    
    return 0;
}
