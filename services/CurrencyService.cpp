#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "CurrencyService.h"
#include <httplib.h>
#include <pugixml.hpp>
#include <spdlog/spdlog.h> 
#include <string>
#include <algorithm>

CurrencyService::CurrencyService(Database& db) : database(db) {}

void CurrencyService::updateRatesFromCBR() {
    httplib::Client cli("https://www.cbr-xml-daily.ru");
    
    spdlog::info("[CurrencyService] Fetching latest exchange rates from CBR (XML)...");
    
    if (auto res = cli.Get("/daily_utf8.xml")) {
        if (res->status == 200) {
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_string(res->body.c_str());
            
            if (result) {
                double usd_rate = 0.0;
                double eur_rate = 0.0;
                
                for (pugi::xml_node valute = doc.child("ValCurs").child("Valute"); valute; valute = valute.next_sibling("Valute")) {
                    std::string charCode = valute.child_value("CharCode");
                    
                    if (charCode == "USD" || charCode == "EUR") {
                        std::string valueStr = valute.child_value("Value");
                        std::replace(valueStr.begin(), valueStr.end(), ',', '.');
                        double rate = std::stod(valueStr);
                        
                        if (charCode == "USD") usd_rate = rate;
                        if (charCode == "EUR") eur_rate = rate;
                        
                        database.updateExchangeRate(charCode, rate);
                    }
                }
                
                database.updateExchangeRate("RUB", 1.0); 
                spdlog::info("[CurrencyService] Success! USD: {} RUB, EUR: {} RUB", usd_rate, eur_rate);
            } else {
                spdlog::error("[CurrencyService] XML Parse error: {}", result.description());
            }
        } else {
            spdlog::error("[CurrencyService] HTTP Error: {}", res->status);
        }
    } else {
        spdlog::error("[CurrencyService] Failed to connect to CBR API");
    }
}
