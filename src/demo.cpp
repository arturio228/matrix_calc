#include "demo.h"

#include "matrix.h"

#include <array>
#include <iostream>

namespace matrix {

void run_demo() {
    double first[] = {1, 2, 3, 4};
    double second[] = {5, 6, 7, 8};
    std::array<Matrix, 6> matrices{};
    try {
        matrices[0] = matrix_from_array(first, 2, 2);
        matrices[1] = matrix_from_array(second, 2, 2);
        matrices[2] = matrix_add(matrices[0], matrices[1]);
        matrices[3] = matrix_multiply(matrices[0], matrices[1]);
        matrices[4] = matrix_transpose(matrices[0]);
        matrices[5] = create_matrix(2, 3);
        matrix_fill(matrices[5], 2.5);
        std::cout << "матрица a:\n";
        print_matrix(matrices[0]);
        std::cout << "матрица b:\n";
        print_matrix(matrices[1]);
        std::cout << "сумма:\n";
        print_matrix(matrices[2]);
        std::cout << "произведение:\n";
        print_matrix(matrices[3]);
        std::cout << "транспонирование a:\n";
        print_matrix(matrices[4]);
        std::cout << "заполнение 2.5:\n";
        print_matrix(matrices[5]);
    } catch (...) {
        for (Matrix m : matrices) {
            free_matrix(m);
        }
        throw;
    }
    for (Matrix m : matrices) {
        free_matrix(m);
    }
}

} // namespace matrix
