FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libsqlite3-dev \
    nlohmann-json3-dev \
    libboost-all-dev

WORKDIR /app

COPY . .

RUN g++ main.cpp \
database/Database.cpp \
services/UserService.cpp \
services/TransactionService.cpp \
handlers/UserHandler.cpp \
handlers/TransactionHandler.cpp \
server/Server.cpp \
-lsqlite3 \
-std=c++17 \
-o budget_app

CMD ["./budget_app"]