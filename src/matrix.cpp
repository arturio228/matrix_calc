#include "matrix.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace matrix {
namespace {

/** @brief Максимальное число видимых строк. */
inline constexpr int kRowsToShow = 10;
/** @brief Максимальное число видимых столбцов. */
inline constexpr int kColsToShow = 8;
/** @brief Минимальная ширина числового поля. */
inline constexpr int kFieldWidth = 8;
/** @brief Число знаков после точки. */
inline constexpr int kPrecision = 2;

/** @brief Проверяет размеры до выделения памяти.
 * @param rows Число строк.
 * @param cols Число столбцов.
 * @throws std::invalid_argument При неположительных или чрезмерных размерах.
 */
void check_dimensions(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("размеры должны быть положительными");
    }
    auto max_size = std::numeric_limits<std::size_t>::max();
    auto row_count = static_cast<std::size_t>(rows);
    auto col_count = static_cast<std::size_t>(cols);
    if (row_count > max_size / sizeof(double*) ||
        col_count > max_size / sizeof(double) / row_count) {
        throw std::invalid_argument("слишком большая матрица");
    }
}

/** @brief Проверяет указатели и при необходимости конечность элементов.
 * @param m Проверяемая матрица.
 * @param check_values Нужно ли проверять элементы.
 * @throws std::invalid_argument При неверной структуре или NaN/Inf.
 */
void check_matrix(Matrix m, bool check_values = true) {
    check_dimensions(m.rows, m.cols);
    if (m.data == nullptr) {
        throw std::invalid_argument("пустой указатель матрицы");
    }
    for (int i = 0; i < m.rows; ++i) {
        if (m.data[i] == nullptr) {
            throw std::invalid_argument("пустой указатель строки");
        }
        if (check_values) {
            for (int j = 0; j < m.cols; ++j) {
                if (!std::isfinite(m.data[i][j])) {
                    throw std::invalid_argument("элемент должен быть конечным числом");
                }
            }
        }
    }
}

/** @brief Проверяет результат арифметики double.
 * @param value Результат операции.
 * @return То же конечное значение.
 * @throws std::overflow_error При бесконечности или NaN.
 */
double check_result(double value) {
    if (!std::isfinite(value)) {
        throw std::overflow_error("переполнение результата");
    }
    return value;
}

} // namespace

Matrix create_matrix(int rows, int cols) {
    check_dimensions(rows, cols);
    using RowPointer = double*;
    Matrix result{new RowPointer[rows]{}, rows, cols};
    try {
        for (int i = 0; i < rows; ++i) {
            result.data[i] = new double[cols]{};
        }
    } catch (...) {
        free_matrix(result);
        throw;
    }
    return result;
}

void free_matrix(Matrix m) noexcept {
    if (m.data == nullptr) {
        return;
    }
    for (int i = 0; i < m.rows; ++i) {
        delete[] m.data[i];
    }
    delete[] m.data;
}

Matrix matrix_from_array(double* data, int rows, int cols) {
    check_dimensions(rows, cols);
    if (data == nullptr) {
        throw std::invalid_argument("пустой указатель массива");
    }
    Matrix result = create_matrix(rows, cols);
    try {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                auto index = static_cast<std::size_t>(i) * cols + j;
                if (!std::isfinite(data[index])) {
                    throw std::invalid_argument("элемент должен быть конечным числом");
                }
                result.data[i][j] = data[index];
            }
        }
    } catch (...) {
        free_matrix(result);
        throw;
    }
    return result;
}

Matrix matrix_add(Matrix a, Matrix b) {
    check_matrix(a);
    check_matrix(b);
    if (a.rows != b.rows || a.cols != b.cols) {
        throw std::invalid_argument("размеры матриц не совпадают");
    }
    Matrix result = create_matrix(a.rows, a.cols);
    try {
        for (int i = 0; i < a.rows; ++i) {
            for (int j = 0; j < a.cols; ++j) {
                result.data[i][j] = check_result(a.data[i][j] + b.data[i][j]);
            }
        }
    } catch (...) {
        free_matrix(result);
        throw;
    }
    return result;
}

Matrix matrix_multiply(Matrix a, Matrix b) {
    check_matrix(a);
    check_matrix(b);
    if (a.cols != b.rows) {
        throw std::invalid_argument("несовместимые размеры матриц");
    }
    Matrix result = create_matrix(a.rows, b.cols);
    try {
        for (int i = 0; i < a.rows; ++i) {
            for (int k = 0; k < a.cols; ++k) {
                for (int j = 0; j < b.cols; ++j) {
                    double product = check_result(a.data[i][k] * b.data[k][j]);
                    result.data[i][j] = check_result(result.data[i][j] + product);
                }
            }
        }
    } catch (...) {
        free_matrix(result);
        throw;
    }
    return result;
}

void matrix_fill(Matrix m, double value) {
    check_matrix(m, false);
    if (!std::isfinite(value)) {
        throw std::invalid_argument("значение должно быть конечным числом");
    }
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            m.data[i][j] = value;
        }
    }
}

Matrix matrix_transpose(Matrix m) {
    check_matrix(m);
    Matrix result = create_matrix(m.cols, m.rows);
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            result.data[j][i] = m.data[i][j];
        }
    }
    return result;
}

void print_matrix(Matrix m) {
    if (m.data == nullptr && m.rows == 0 && m.cols == 0) {
        std::cout << "пустая матрица\n";
        return;
    }
    check_matrix(m);
    auto old_flags = std::cout.flags();
    auto old_precision = std::cout.precision();
    std::cout << std::fixed << std::setprecision(kPrecision);
    for (int i = 0; i < m.rows; ++i) {
        if (m.rows > kRowsToShow && i == kRowsToShow / 2) {
            std::cout << "пропущено строк: " << m.rows - kRowsToShow << '\n';
            i = m.rows - kRowsToShow / 2;
        }
        std::cout << '[';
        for (int j = 0; j < m.cols; ++j) {
            if (m.cols > kColsToShow && j == kColsToShow / 2) {
                std::cout << " ...";
                j = m.cols - kColsToShow / 2;
            }
            std::cout << std::setw(kFieldWidth) << m.data[i][j];
        }
        std::cout << " ]\n";
    }
    if (m.rows > kRowsToShow || m.cols > kColsToShow) {
        std::cout << "размер: " << m.rows << " x " << m.cols << '\n';
    }
    std::cout.flags(old_flags);
    std::cout.precision(old_precision);
}

} // namespace matrix
