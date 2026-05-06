FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libsqlite3-dev \
    nlohmann-json3-dev \
    libboost-all-dev \
    libbotan-2-dev \
    wget \
    libssl-dev \
    libpugixml-dev \
    libspdlog-dev

WORKDIR /app

RUN wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -O /usr/include/httplib.h

COPY . .

RUN g++ main.cpp \
database/Database.cpp \
services/UserService.cpp \
services/TransactionService.cpp \
services/CurrencyService.cpp \
services/CryptoHelper.cpp \
services/MLCategorizer.cpp \
handlers/UserHandler.cpp \
handlers/TransactionHandler.cpp \
server/Server.cpp \
server/Session.cpp \
-I/usr/include/botan-2 \
-lsqlite3 \
-lbotan-2 \
-lssl \
-lcrypto \
-lpugixml \
-lspdlog \
-lfmt \
-pthread \
-std=c++20 \
-o budget_app

CMD ["./budget_app"]