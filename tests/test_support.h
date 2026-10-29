#pragma once

#include "matrix.h"

namespace matrix::tests {

/** @brief Единственный владелец матрицы в тесте, включая выход через ASSERT.
 */
struct OwnedMatrix {
    Matrix value;

    /** @brief Принимает владение.
     * @param m Новая матрица из библиотечной функции.
     */
    explicit OwnedMatrix(Matrix m) : value(m) {}

    /** @brief Освобождает матрицу при любом выходе из теста. */
    ~OwnedMatrix() {
        free_matrix(value);
    }

    OwnedMatrix(OwnedMatrix&) = delete;
    OwnedMatrix& operator=(OwnedMatrix&) = delete;
};

} // namespace matrix::tests
