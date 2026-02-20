#include "UserManager.h"
#include <iostream>
#include <windows.h>
#include <locale>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::locale::global(std::locale(""));

    UserManager manager;
    manager.loadUsers();

    int choice;

    do {
        std::cout << "\n1. Регистрация\n2. Вход\n0. Выход\nВыбор: ";
        std::cin >> choice;

        if (choice == 1) {
            std::string u, p;
            std::cout << "Login: ";
            std::cin >> u;
            std::cout << "Password: ";
            std::cin >> p;

            if (manager.registerUser(u, p))
                std::cout << "Ok\n";
            else
                std::cout << "уже существует с таким именем\n";
        }

        else if (choice == 2) {
            std::string u, p;
            std::cout << "Login: ";
            std::cin >> u;
            std::cout << "Password: ";
            std::cin >> p;

            if (manager.login(u, p)) {
                std::cout << "Вход выполнен\n";

                User* user = manager.getCurrentUser();
                int userChoice;

                do {
                    std::cout << "\n1. Добавить операцию\n";
                    std::cout << "2. Показать операции\n";
                    std::cout << "3. Баланс\n";
                    std::cout << "0. Выйти\nВыбор: ";
                    std::cin >> userChoice;

                    if (userChoice == 1) {
                        Transaction t;
                        int typeInput;

                        std::cout << "Тип (1-Доход, 2-Расход): ";
                        std::cin >> typeInput;
                        t.type = (typeInput == 1) ? Type::Income : Type::Expense;

                        std::cout << "Сумма: ";
                        std::cin >> t.amount;

                        std::cout << "Категория: ";
                        std::cin >> t.category;

                        std::cout << "Дата: ";
                        std::cin >> t.date;

                        user->budget.addTransaction(t);
                        user->budget.saveToFile("data_" + user->username + ".txt");
                    }

                    else if (userChoice == 2) {
                        user->budget.showTransactions();
                    }

                    else if (userChoice == 3) {
                        std::cout << "Баланс: "
                                  << user->budget.calculateBalance()
                                  << "\n";
                    }

                } while (userChoice != 0);
            }
            else {
                std::cout << "Неверный логин или пароль\n";
            }
        }

    } while (choice != 0);

    return 0;
}
