#include "matrix.h"

#include "test_support.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace matrix::tests {

/** @brief Проверяет условие без зависимости от assert и NDEBUG.
 * @param condition Условие успешной проверки.
 * @param message Описание ошибки.
 * @return Ничего.
 * @throws std::runtime_error При провале проверки.
 */
void require(bool condition, std::string message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

/** @brief Проверяет сложение и умножение на известных матрицах.
 * @return Ничего.
 * @throws std::exception При ошибке проверки.
 */
void test_arithmetic() {
    double first[] = {1, 2, 3, 4};
    double second[] = {5, 6, 7, 8};
    OwnedMatrix a(matrix_from_array(first, 2, 2));
    OwnedMatrix b(matrix_from_array(second, 2, 2));
    OwnedMatrix sum(matrix_add(a.value, b.value));
    OwnedMatrix product(matrix_multiply(a.value, b.value));
    double expected_sum[] = {6, 8, 10, 12};
    double expected_product[] = {19, 22, 43, 50};
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            require(sum.value.data[i][j] == expected_sum[2 * i + j], "сумма");
            require(product.value.data[i][j] == expected_product[2 * i + j], "произведение");
        }
    }
}

/** @brief Проверяет вариант 8 и взаимодействие операций.
 * @return Ничего.
 * @throws std::exception При ошибке проверки.
 */
void test_fill_and_integration() {
    OwnedMatrix a(create_matrix(2, 3));
    double** address = a.value.data;
    matrix_fill(a.value, 2.5);
    require(a.value.data == address && a.value.rows == 2 && a.value.cols == 3, "размер");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            require(a.value.data[i][j] == 2.5, "заполнение");
        }
    }
    OwnedMatrix t(matrix_transpose(a.value));
    OwnedMatrix product(matrix_multiply(a.value, t.value));
    require(product.value.data[1][1] == 18.75, "интеграция");
    matrix_fill(a.value, -1);
    require(a.value.data[1][2] == -1, "отрицательная константа");
    matrix_fill(a.value, 0);
    require(a.value.data[1][2] == 0, "нулевая константа");
}

/** @brief Проверяет минимальную матрицу и независимое копирование.
 * @return Ничего.
 * @throws std::exception При ошибке проверки.
 */
void test_boundaries() {
    double value[] = {7};
    OwnedMatrix a(matrix_from_array(value, 1, 1));
    value[0] = 100;
    require(a.value.data[0][0] == 7, "копирование");
    OwnedMatrix t(matrix_transpose(a.value));
    require(t.value.rows == 1 && t.value.cols == 1 && t.value.data[0][0] == 7,
            "минимальный размер");
    free_matrix(Matrix{});
    free_matrix(Matrix{});
}

/** @brief Проверяет отказы при неверных аргументах.
 * @return Ничего.
 * @throws std::exception При ошибке проверки.
 */
void test_invalid_arguments() {
    int caught = 0;
    try {
        create_matrix(0, 2);
    } catch (std::invalid_argument&) {
        ++caught;
    }
    try {
        create_matrix(2, -1);
    } catch (std::invalid_argument&) {
        ++caught;
    }
    try {
        matrix_from_array(nullptr, 2, 2);
    } catch (std::invalid_argument&) {
        ++caught;
    }
    try {
        matrix_fill(Matrix{}, 2.5);
    } catch (std::invalid_argument&) {
        ++caught;
    }
    OwnedMatrix a(create_matrix(2, 3));
    OwnedMatrix b(create_matrix(2, 2));
    try {
        matrix_add(a.value, b.value);
    } catch (std::invalid_argument&) {
        ++caught;
    }
    try {
        matrix_multiply(a.value, b.value);
    } catch (std::invalid_argument&) {
        ++caught;
    }
    require(caught == 6, "проверки аргументов");
}

/** @brief Проверяет NaN, бесконечность и переполнение.
 * @return Ничего.
 * @throws std::exception При ошибке проверки.
 */
void test_nonfinite_and_overflow() {
    OwnedMatrix a(create_matrix(1, 1));
    int caught = 0;
    try {
        matrix_fill(a.value, std::numeric_limits<double>::infinity());
    } catch (std::invalid_argument&) {
        ++caught;
    }
    matrix_fill(a.value, std::numeric_limits<double>::max());
    try {
        matrix_add(a.value, a.value);
    } catch (std::overflow_error&) {
        ++caught;
    }
    try {
        matrix_multiply(a.value, a.value);
    } catch (std::overflow_error&) {
        ++caught;
    }
    require(caught == 3, "проверки конечности и переполнения");
}

/** @brief Запускает пять автономных сценариев.
 * @return Ничего.
 * @throws std::exception При ошибке проверки.
 */
void run_smoke_tests() {
    test_arithmetic();
    std::cout << "арифметика: ok\n";
    test_fill_and_integration();
    std::cout << "вариант 8 и интеграция: ok\n";
    test_boundaries();
    std::cout << "граничные случаи: ok\n";
    test_invalid_arguments();
    std::cout << "неверные аргументы: ok\n";
    test_nonfinite_and_overflow();
    std::cout << "конечность и переполнение: ok\n";
    std::cout << "проверок: 5, ошибок: 0\n";
}

} // namespace matrix::tests

/** @brief Запускает автономные проверки без Google Test.
 * @return 0 при успехе, 1 при провале.
 */
int main() {
    try {
        matrix::tests::run_smoke_tests();
        return 0;
    } catch (std::exception& error) {
        std::cerr << "ошибка: " << error.what() << '\n';
        return 1;
    }
}
