#include "matrix.h"

#include "test_support.h"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <string>

namespace matrix::tests {

TEST(MatrixTest, CreateZeroInitialized) {
    OwnedMatrix a(create_matrix(3, 4));
    EXPECT_EQ(a.value.rows, 3);
    EXPECT_EQ(a.value.cols, 4);
    for (int i = 0; i < a.value.rows; ++i) {
        for (int j = 0; j < a.value.cols; ++j) {
            EXPECT_DOUBLE_EQ(a.value.data[i][j], 0.0);
        }
    }
}

TEST(MatrixTest, MinimumSize) {
    OwnedMatrix a(create_matrix(1, 1));
    EXPECT_DOUBLE_EQ(a.value.data[0][0], 0.0);
}

TEST(MatrixTest, ZeroRowsRejected) {
    EXPECT_THROW(create_matrix(0, 3), std::invalid_argument);
}

TEST(MatrixTest, NegativeColumnsRejected) {
    EXPECT_THROW(create_matrix(3, -1), std::invalid_argument);
}

TEST(MatrixTest, EmptyFreeIsSafe) {
    Matrix empty{};
    EXPECT_NO_THROW(free_matrix(empty));
    EXPECT_NO_THROW(free_matrix(empty));
}

TEST(MatrixTest, FromArrayCopiesIndependently) {
    double values[] = {1, 2, 3, 4, 5, 6};
    OwnedMatrix a(matrix_from_array(values, 2, 3));
    values[0] = 100;
    EXPECT_DOUBLE_EQ(a.value.data[0][0], 1.0);
    EXPECT_DOUBLE_EQ(a.value.data[1][2], 6.0);
}

TEST(MatrixTest, NullArrayRejected) {
    EXPECT_THROW(matrix_from_array(nullptr, 2, 2), std::invalid_argument);
}

TEST(MatrixTest, NonFiniteArrayRejected) {
    double values[] = {1, std::numeric_limits<double>::infinity()};
    EXPECT_THROW(matrix_from_array(values, 1, 2), std::invalid_argument);
}

TEST(MatrixTest, Addition) {
    double first[] = {1, 2, 3, 4};
    double second[] = {5, 6, 7, 8};
    OwnedMatrix a(matrix_from_array(first, 2, 2));
    OwnedMatrix b(matrix_from_array(second, 2, 2));
    OwnedMatrix c(matrix_add(a.value, b.value));
    EXPECT_DOUBLE_EQ(c.value.data[0][0], 6.0);
    EXPECT_DOUBLE_EQ(c.value.data[0][1], 8.0);
    EXPECT_DOUBLE_EQ(c.value.data[1][0], 10.0);
    EXPECT_DOUBLE_EQ(c.value.data[1][1], 12.0);
    EXPECT_DOUBLE_EQ(a.value.data[0][0], 1.0);
}

TEST(MatrixTest, AdditionMismatchRejected) {
    OwnedMatrix a(create_matrix(2, 2));
    OwnedMatrix b(create_matrix(3, 3));
    EXPECT_THROW(matrix_add(a.value, b.value), std::invalid_argument);
}

TEST(MatrixTest, NullMatrixRejected) {
    Matrix invalid{nullptr, 2, 2};
    OwnedMatrix a(create_matrix(2, 2));
    EXPECT_THROW(matrix_add(invalid, a.value), std::invalid_argument);
    EXPECT_THROW(matrix_multiply(a.value, invalid), std::invalid_argument);
    EXPECT_THROW(matrix_transpose(invalid), std::invalid_argument);
}

TEST(MatrixTest, NullRowRejected) {
    double* rows[] = {nullptr};
    Matrix invalid{rows, 1, 1};
    EXPECT_THROW(matrix_fill(invalid, 2.5), std::invalid_argument);
}

TEST(MatrixTest, Multiplication) {
    double first[] = {1, 2, 3, 4};
    double second[] = {5, 6, 7, 8};
    OwnedMatrix a(matrix_from_array(first, 2, 2));
    OwnedMatrix b(matrix_from_array(second, 2, 2));
    OwnedMatrix c(matrix_multiply(a.value, b.value));
    EXPECT_DOUBLE_EQ(c.value.data[0][0], 19.0);
    EXPECT_DOUBLE_EQ(c.value.data[0][1], 22.0);
    EXPECT_DOUBLE_EQ(c.value.data[1][0], 43.0);
    EXPECT_DOUBLE_EQ(c.value.data[1][1], 50.0);
}

TEST(MatrixTest, RectangularMultiplication) {
    double first[] = {1, 2, 3, 4, 5, 6};
    double second[] = {7, 8, 9, 10, 11, 12};
    OwnedMatrix a(matrix_from_array(first, 2, 3));
    OwnedMatrix b(matrix_from_array(second, 3, 2));
    OwnedMatrix c(matrix_multiply(a.value, b.value));
    EXPECT_EQ(c.value.rows, 2);
    EXPECT_EQ(c.value.cols, 2);
    EXPECT_DOUBLE_EQ(c.value.data[0][0], 58.0);
    EXPECT_DOUBLE_EQ(c.value.data[0][1], 64.0);
    EXPECT_DOUBLE_EQ(c.value.data[1][0], 139.0);
    EXPECT_DOUBLE_EQ(c.value.data[1][1], 154.0);
}

TEST(MatrixTest, RowTimesColumn) {
    double first[] = {1, 2, 3};
    double second[] = {4, 5, 6};
    OwnedMatrix a(matrix_from_array(first, 1, 3));
    OwnedMatrix b(matrix_from_array(second, 3, 1));
    OwnedMatrix c(matrix_multiply(a.value, b.value));
    EXPECT_EQ(c.value.rows, 1);
    EXPECT_EQ(c.value.cols, 1);
    EXPECT_DOUBLE_EQ(c.value.data[0][0], 32.0);
}

TEST(MatrixTest, MultiplicationMismatchRejected) {
    OwnedMatrix a(create_matrix(2, 3));
    OwnedMatrix b(create_matrix(2, 2));
    EXPECT_THROW(matrix_multiply(a.value, b.value), std::invalid_argument);
}

TEST(MatrixTest, TransposeRectangular) {
    double values[] = {1, 2, 3, 4, 5, 6};
    OwnedMatrix a(matrix_from_array(values, 2, 3));
    OwnedMatrix t(matrix_transpose(a.value));
    EXPECT_EQ(t.value.rows, 3);
    EXPECT_EQ(t.value.cols, 2);
    EXPECT_DOUBLE_EQ(t.value.data[0][1], 4.0);
    EXPECT_DOUBLE_EQ(t.value.data[2][1], 6.0);
    t.value.data[0][0] = 100;
    EXPECT_DOUBLE_EQ(a.value.data[0][0], 1.0);
}

TEST(MatrixTest, TransposeTwice) {
    double values[] = {1, 2, 3, 4, 5, 6};
    OwnedMatrix a(matrix_from_array(values, 2, 3));
    OwnedMatrix t(matrix_transpose(a.value));
    OwnedMatrix tt(matrix_transpose(t.value));
    for (int i = 0; i < a.value.rows; ++i) {
        for (int j = 0; j < a.value.cols; ++j) {
            EXPECT_DOUBLE_EQ(tt.value.data[i][j], a.value.data[i][j]);
        }
    }
}

TEST(MatrixTest, FillConstantPreservesAllocationAndSize) {
    OwnedMatrix a(create_matrix(2, 3));
    double** address = a.value.data;
    double* row = a.value.data[0];
    matrix_fill(a.value, 2.5);
    EXPECT_EQ(a.value.data, address);
    EXPECT_EQ(a.value.data[0], row);
    EXPECT_EQ(a.value.rows, 2);
    EXPECT_EQ(a.value.cols, 3);
    for (int i = 0; i < a.value.rows; ++i) {
        for (int j = 0; j < a.value.cols; ++j) {
            EXPECT_DOUBLE_EQ(a.value.data[i][j], 2.5);
        }
    }
}

TEST(MatrixTest, FillZeroNegativeAndSingleElement) {
    OwnedMatrix a(create_matrix(1, 1));
    matrix_fill(a.value, -3.25);
    EXPECT_DOUBLE_EQ(a.value.data[0][0], -3.25);
    matrix_fill(a.value, 0);
    EXPECT_DOUBLE_EQ(a.value.data[0][0], 0);
}

TEST(MatrixTest, FillEmptyRejected) {
    EXPECT_THROW(matrix_fill(Matrix{}, 2.5), std::invalid_argument);
}

TEST(MatrixTest, FillNonFiniteRejectedWithoutMutation) {
    OwnedMatrix a(create_matrix(2, 2));
    matrix_fill(a.value, 3);
    EXPECT_THROW(matrix_fill(a.value, std::numeric_limits<double>::quiet_NaN()),
                 std::invalid_argument);
    EXPECT_THROW(matrix_fill(a.value, std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
    EXPECT_DOUBLE_EQ(a.value.data[1][1], 3.0);
}

TEST(MatrixTest, FillReplacesNonFiniteOldValues) {
    OwnedMatrix a(create_matrix(1, 1));
    a.value.data[0][0] = std::numeric_limits<double>::quiet_NaN();
    matrix_fill(a.value, 4);
    EXPECT_DOUBLE_EQ(a.value.data[0][0], 4.0);
}

TEST(MatrixTest, AdditionOverflowRejected) {
    OwnedMatrix a(create_matrix(1, 1));
    matrix_fill(a.value, std::numeric_limits<double>::max());
    EXPECT_THROW(matrix_add(a.value, a.value), std::overflow_error);
}

TEST(MatrixTest, MultiplicationOverflowRejected) {
    OwnedMatrix a(create_matrix(1, 1));
    matrix_fill(a.value, std::numeric_limits<double>::max());
    EXPECT_THROW(matrix_multiply(a.value, a.value), std::overflow_error);
}

TEST(MatrixTest, PrintEmpty) {
    testing::internal::CaptureStdout();
    print_matrix(Matrix{});
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "пустая матрица\n");
}

TEST(MatrixTest, PrintLargeIncludesLastElements) {
    OwnedMatrix a(create_matrix(15, 12));
    a.value.data[14][11] = 99.25;
    testing::internal::CaptureStdout();
    print_matrix(a.value);
    std::string text = testing::internal::GetCapturedStdout();
    EXPECT_NE(text.find("99.25"), std::string::npos);
    EXPECT_NE(text.find("пропущено строк: 5"), std::string::npos);
    EXPECT_NE(text.find("размер: 15 x 12"), std::string::npos);
}

TEST(MatrixIntegration, FillAddMultiplyTranspose) {
    OwnedMatrix a(create_matrix(2, 3));
    matrix_fill(a.value, 2.5);
    OwnedMatrix t(matrix_transpose(a.value));
    OwnedMatrix product(matrix_multiply(a.value, t.value));
    OwnedMatrix sum(matrix_add(product.value, product.value));
    EXPECT_EQ(sum.value.rows, 2);
    EXPECT_EQ(sum.value.cols, 2);
    for (int i = 0; i < sum.value.rows; ++i) {
        for (int j = 0; j < sum.value.cols; ++j) {
            EXPECT_DOUBLE_EQ(sum.value.data[i][j], 37.5);
        }
    }
}

} // namespace matrix::tests
