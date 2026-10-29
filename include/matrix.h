#pragma once

/** @file matrix.h
 * @brief Операции калькулятора матриц и вариант 8.
 */
/** @brief Библиотека и демонстрация калькулятора матриц. */
namespace matrix {

/** @brief Динамическая матрица с единственным владельцем памяти.
 * @warning Копия структуры разделяет память с оригиналом. Освобождайте ее один раз.
 */
struct Matrix {
    double** data = nullptr; ///< Указатели на строки.
    int rows = 0;            ///< Число строк.
    int cols = 0;            ///< Число столбцов.
};

/** @brief Создает нулевую матрицу.
 * @param rows Положительное число строк.
 * @param cols Положительное число столбцов.
 * @return Новая матрица, которую нужно освободить.
 * @throws std::invalid_argument При неположительных размерах.
 * @throws std::bad_alloc При нехватке памяти; частичные выделения очищаются.
 */
Matrix create_matrix(int rows, int cols);

/** @brief Освобождает строки и массив указателей.
 * @param m Матрица, созданная библиотекой, либо пустая структура.
 * @note nullptr безопасен. После вызова присвойте исходной переменной {}.
 * @warning Передача по значению не обнуляет оригинал и его копии.
 */
void free_matrix(Matrix m) noexcept;

/** @brief Копирует одномерный массив по строкам.
 * @param data Массив как минимум из rows * cols конечных чисел.
 * @param rows Положительное число строк.
 * @param cols Положительное число столбцов.
 * @return Независимая матрица с выделенной памятью.
 * @throws std::invalid_argument При nullptr, неверном размере или NaN/Inf.
 * @throws std::bad_alloc При нехватке памяти.
 */
Matrix matrix_from_array(double* data, int rows, int cols);

/** @brief Складывает матрицы одинакового размера.
 * @param a Первая матрица.
 * @param b Вторая матрица.
 * @return Новая матрица суммы; входные матрицы не меняются.
 * @throws std::invalid_argument При неверных данных или разных размерах.
 * @throws std::overflow_error При неограниченном результате.
 * @throws std::bad_alloc При нехватке памяти.
 */
Matrix matrix_add(Matrix a, Matrix b);

/** @brief Умножает матрицы циклом i, k, j.
 * @param a Матрица размера M на N.
 * @param b Матрица размера N на K.
 * @return Новая матрица размера M на K; входные матрицы не меняются.
 * @throws std::invalid_argument При неверных данных или несовместимых размерах.
 * @throws std::overflow_error При переполнении произведения или суммы.
 * @throws std::bad_alloc При нехватке памяти.
 */
Matrix matrix_multiply(Matrix a, Matrix b);

/** @brief Заполняет все элементы константой, вариант 8.
 * @param m Изменяемая матрица.
 * @param value Конечное значение для каждого элемента.
 * @note Размер и адреса выделений сохраняются.
 * @throws std::invalid_argument При неверной структуре или NaN/Inf в value.
 * @note Прежние значения элементов заменяются без чтения.
 */
void matrix_fill(Matrix m, double value);

/** @brief Транспонирует матрицу.
 * @param m Исходная матрица.
 * @return Независимая матрица с переставленными строками и столбцами.
 * @throws std::invalid_argument При неверных данных.
 * @throws std::bad_alloc При нехватке памяти.
 */
Matrix matrix_transpose(Matrix m);

/** @brief Печатает матрицу с двумя знаками после точки.
 * @param m Матрица для вывода; полностью пустая структура допустима.
 * @note Большие матрицы сокращаются до первых и последних элементов.
 * @throws std::invalid_argument При поврежденной структуре или NaN/Inf.
 */
void print_matrix(Matrix m);

} // namespace matrix
