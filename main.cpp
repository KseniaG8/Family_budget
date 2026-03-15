#include "database/Database.h"
#include "services/TransactionService.h"
#include "handlers/TransactionHandler.h"
#include "services/UserService.h"
#include "handlers/UserHandler.h"
#include "server/Server.h"
#include <boost/asio.hpp>
#include <iostream>

int main() {
    Database db("budget.db");
    db.init();

    UserService userService(db);
    UserHandler userHandler(userService);
    
    TransactionService service(db);
    TransactionHandler handler(service);

    try {
        boost::asio::io_context io_context;
        Server server(io_context, 8080, userHandler, handler);
        std::cout << "Server running on port 8080...\n";
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
