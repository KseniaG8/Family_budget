#pragma once
#include "../database/Database.h"

class CurrencyService {
public:
    CurrencyService(Database& db);
    
    void updateRatesFromCBR();

private:
    Database& database;
};
