#include "database/Database.h"
#include "handlers/TransactionHandler.h"
#include "handlers/UserHandler.h"
#include "server/Server.h"
#include "services/TransactionService.h"
#include "services/UserService.h"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

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
        std::cout << "HTTP server running on port 8080...\n";
        
        std::vector<std::thread> threads;

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([&io_context]() { io_context.run(); });
        }
        for (auto &t : threads) {
            t.join();
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
