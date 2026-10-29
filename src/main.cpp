#include "demo.h"

#include <exception>
#include <iostream>

/** @brief Запускает демонстрацию калькулятора.
 * @return 0 при успехе, 1 при ошибке.
 */
int main() {
    try {
        matrix::run_demo();
        return 0;
    } catch (std::exception& error) {
        std::cerr << "ошибка: " << error.what() << '\n';
        return 1;
    }
}
