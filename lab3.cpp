#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <clocale>
#include <cassert>

#ifdef _WIN32
    #include <windows.h>
#endif

// ==================== Настройка для работы с русским языком ====================
void setup_russian_locale() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
}
// ==================== Шаблон класса Matrix ====================
template<typename T>
class Matrix {
    // Статическая проверка: T должен быть арифметическим типом
    static_assert(std::is_arithmetic<T>::value,
                  "Matrix can only be instantiated with arithmetic types");

private:
    T** data;        // Указатель на двумерный массив
    size_t rows;     // Количество строк
    size_t cols;     // Количество столбцов

    // Вспомогательный метод для выделения памяти
    void allocate_memory() {
        data = new T*[rows];
        for (size_t i = 0; i < rows; ++i) {
            data[i] = new T[cols]();
        }
    }

    // Вспомогательный метод для освобождения памяти
    void deallocate_memory() {
        if (data) {
            for (size_t i = 0; i < rows; ++i) {
                delete[] data[i];
            }
            delete[] data;
            data = nullptr;
        }
    }

    // Вспомогательный метод для копирования данных
    void copy_from(const Matrix& other) {
        rows = other.rows;
        cols = other.cols;
        allocate_memory();
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
    }

public:
    // ==================== Конструкторы и деструктор ====================

    // Конструктор по умолчанию (создает пустую матрицу 0x0)
    Matrix() : data(nullptr), rows(0), cols(0) {}

    // Конструктор с размерами
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        if (rows == 0 || cols == 0) {
            this->rows = 0;
            this->cols = 0;
            data = nullptr;
        } else {
            allocate_memory();
        }
    }

    // Конструктор с размерами и значением для заполнения
    Matrix(size_t rows, size_t cols, const T& value) : rows(rows), cols(cols) {
        if (rows == 0 || cols == 0) {
            this->rows = 0;
            this->cols = 0;
            data = nullptr;
        } else {
            allocate_memory();
            for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                    data[i][j] = value;
                }
            }
        }
    }

    // Конструктор копирования
    Matrix(const Matrix& other) : data(nullptr), rows(0), cols(0) {
        copy_from(other);
    }

    // Конструктор перемещения
    Matrix(Matrix&& other) noexcept
        : data(other.data), rows(other.rows), cols(other.cols) {
        other.data = nullptr;
        other.rows = 0;
        other.cols = 0;
    }

    // Деструктор
    ~Matrix() {
        deallocate_memory();
    }

    // ==================== Операторы присваивания ====================

    // Копирующее присваивание
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            Matrix temp(other);  // Copy-and-swap идиома
            swap(temp);
        }
        return *this;
    }

    // Перемещающее присваивание
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            deallocate_memory();
            data = other.data;
            rows = other.rows;
            cols = other.cols;
            other.data = nullptr;
            other.rows = 0;
            other.cols = 0;
        }
        return *this;
    }

    // ==================== Доступ к элементам ====================

    // Не константный доступ (с проверкой границ)
    T& at(size_t row, size_t col) {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Matrix index out of range");
        }
        return data[row][col];
    }

    // Константный доступ (с проверкой границ)
    const T& at(size_t row, size_t col) const {
        if (row >= rows || col >= cols) {
            throw std::out_of_range("Matrix index out of range");
        }
        return data[row][col];
    }

    // Оператор доступа без проверки границ (неконстантный)
    T* operator[](size_t row) {
        return data[row];
    }

    // Оператор доступа без проверки границ (константный)
    const T* operator[](size_t row) const {
        return data[row];
    }

    // ==================== Методы получения информации ====================

    // Получение количества строк
    size_t get_rows() const {
        return rows;
    }

    // Получение количества столбцов
    size_t get_cols() const {
        return cols;
    }

    // Проверка, пустая ли матрица
    bool is_empty() const {
        return rows == 0 || cols == 0;
    }

    // Проверка, квадратная ли матрица
    bool is_square() const {
        return rows == cols;
    }

    // ==================== Операторы сравнения ====================

    // Оператор равенства
    bool operator==(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            return false;
        }

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (data[i][j] != other.data[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    // Оператор неравенства
    bool operator!=(const Matrix& other) const {
        return !(*this == other);
    }

    // ==================== Арифметические операторы ====================

    // Унарный минус
    Matrix operator-() const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = -data[i][j];
            }
        }
        return result;
    }

    // Сложение матриц
    Matrix operator+(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Размеры матриц должны совпадать для сложения");
        }

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    // Вычитание матриц
    Matrix operator-(const Matrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Размеры матриц должны совпадать для вычитания");
        }

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] - other.data[i][j];
            }
        }
        return result;
    }

    // Умножение матриц
    Matrix operator*(const Matrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument(
                "Количество столбцов первой матрицы должно равняться количеству строк второй");
        }

        Matrix result(rows, other.cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                T sum = T(0);
                for (size_t k = 0; k < cols; ++k) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }
        return result;
    }

    // ==================== Операторы с присваиванием ====================

    // Сложение с присваиванием
    Matrix& operator+=(const Matrix& other) {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Размеры матриц должны совпадать для сложения");
        }

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] += other.data[i][j];
            }
        }
        return *this;
    }

    // Вычитание с присваиванием
    Matrix& operator-=(const Matrix& other) {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Размеры матриц должны совпадать для вычитания");
        }

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] -= other.data[i][j];
            }
        }
        return *this;
    }

    // Умножение с присваиванием
    Matrix& operator*=(const Matrix& other) {
        *this = *this * other;
        return *this;
    }

    // ==================== Скалярные операции ====================

    // Умножение на скаляр
    Matrix operator*(const T& scalar) const {
        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] * scalar;
            }
        }
        return result;
    }

    // Деление на скаляр
    Matrix operator/(const T& scalar) const {
        if (scalar == T(0)) {
            throw std::invalid_argument("Деление на ноль");
        }

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] / scalar;
            }
        }
        return result;
    }

    // Умножение на скаляр с присваиванием
    Matrix& operator*=(const T& scalar) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] *= scalar;
            }
        }
        return *this;
    }

    // Деление на скаляр с присваиванием
    Matrix& operator/=(const T& scalar) {
        if (scalar == T(0)) {
            throw std::invalid_argument("Деление на ноль");
        }

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] /= scalar;
            }
        }
        return *this;
    }

    // ==================== Методы для работы с матрицей ====================

    // Заполнение матрицы значением
    void fill(const T& value) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = value;
            }
        }
    }

    // Обмен содержимого с другой матрицей
    void swap(Matrix& other) noexcept {
        std::swap(data, other.data);
        std::swap(rows, other.rows);
        std::swap(cols, other.cols);
    }

    // Транспонирование матрицы
    Matrix transpose() const {
        Matrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[j][i] = data[i][j];
            }
        }
        return result;
    }

    // Получение строки
    T* get_row(size_t row) {
        if (row >= rows) {
            throw std::out_of_range("Индекс строки выходит за пределы");
        }
        return data[row];
    }

    // Получение строки (константная версия)
    const T* get_row(size_t row) const {
        if (row >= rows) {
            throw std::out_of_range("Индекс строки выходит за пределы");
        }
        return data[row];
    }

    // Получение столбца (возвращает новый вектор)
    std::pair<T*, size_t> get_column(size_t col) const {
        if (col >= cols) {
            throw std::out_of_range("Индекс столбца выходит за пределы");
        }

        T* column = new T[rows];
        for (size_t i = 0; i < rows; ++i) {
            column[i] = data[i][col];
        }
        return {column, rows};
    }

    // ==================== Статические методы ====================

    // Создание единичной матрицы
    static Matrix identity(size_t n) {
        Matrix result(n, n);
        for (size_t i = 0; i < n; ++i) {
            result.data[i][i] = T(1);
        }
        return result;
    }

    // Создание нулевой матрицы
    static Matrix zero(size_t rows, size_t cols) {
        return Matrix(rows, cols, T(0));
    }

    // ==================== Итераторы ====================

    // Простой итератор для перебора всех элементов
    class Iterator {
    private:
        T** data;
        size_t rows;
        size_t cols;
        size_t current_row;
        size_t current_col;

    public:
        Iterator(T** data, size_t rows, size_t cols, size_t start_row = 0, size_t start_col = 0)
            : data(data), rows(rows), cols(cols), current_row(start_row), current_col(start_col) {}

        bool operator!=(const Iterator& other) const {
            return current_row != other.current_row || current_col != other.current_col;
        }

        void operator++() {
            ++current_col;
            if (current_col >= cols) {
                current_col = 0;
                ++current_row;
            }
        }

        T& operator*() {
            return data[current_row][current_col];
        }
    };

    Iterator begin() {
        return Iterator(data, rows, cols, 0, 0);
    }

    Iterator end() {
        return Iterator(data, rows, cols, rows, 0);
    }
};

// ==================== Операторы друзья (для удобства) ====================

// Умножение скаляра на матрицу
template<typename T>
Matrix<T> operator*(const T& scalar, const Matrix<T>& matrix) {
    return matrix * scalar;
}

// Вывод матрицы в поток
template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
    os << "Матрица(" << matrix.get_rows() << "x" << matrix.get_cols() << "):\n";
    for (size_t i = 0; i < matrix.get_rows(); ++i) {
        os << "[";
        for (size_t j = 0; j < matrix.get_cols(); ++j) {
            os << matrix[i][j];
            if (j < matrix.get_cols() - 1) os << ", ";
        }
        os << "]\n";
    }
    return os;
}

// Ввод матрицы из потока
template<typename T>
std::istream& operator>>(std::istream& is, Matrix<T>& matrix) {
    std::cout << "Введите элементы матрицы (" << matrix.get_rows()
              << "x" << matrix.get_cols() << "):\n";
    for (size_t i = 0; i < matrix.get_rows(); ++i) {
        for (size_t j = 0; j < matrix.get_cols(); ++j) {
            is >> matrix[i][j];
        }
    }
    return is;
}

// ==================== Функции тестирования ====================

void test_constructor() {
    std::cout << "Тестирование конструкторов...\n";

    Matrix<int> m1;
    assert(m1.is_empty());
    assert(m1.get_rows() == 0);
    assert(m1.get_cols() == 0);

    Matrix<int> m2(3, 4);
    assert(m2.get_rows() == 3);
    assert(m2.get_cols() == 4);

    Matrix<int> m3(3, 4, 5);
    assert(m3[0][0] == 5);
    assert(m3[2][3] == 5);

    std::cout << "OK\n\n";
}

void test_copy_move() {
    std::cout << "Тестирование копирования и перемещения...\n";

    Matrix<int> m1(2, 2, 10);
    Matrix<int> m2 = m1;  // Копирование
    assert(m2[0][0] == 10);
    assert(m2[1][1] == 10);

    Matrix<int> m3 = std::move(m1);  // Перемещение
    assert(m3[0][0] == 10);
    assert(m1.is_empty());

    std::cout << "OK\n\n";
}

void test_access() {
    std::cout << "Тестирование доступа к элементам...\n";

    Matrix<int> m(2, 2);
    m[0][0] = 1;
    m[0][1] = 2;
    m[1][0] = 3;
    m[1][1] = 4;

    assert(m[0][0] == 1);
    assert(m.at(0, 1) == 2);
    assert(m.at(1, 0) == 3);
    assert(m[1][1] == 4);

    bool exception_caught = false;
    try {
        m.at(2, 0);
    } catch (const std::out_of_range&) {
        exception_caught = true;
    }
    assert(exception_caught);

    const Matrix<int>& cm = m;
    assert(cm[0][0] == 1);
    assert(cm.at(1, 1) == 4);

    std::cout << "OK\n\n";
}

void test_arithmetic() {
    std::cout << "Тестирование арифметических операций...\n";

    Matrix<int> A(2, 2, 2);
    Matrix<int> B(2, 2, 3);

    auto sum = A + B;
    assert(sum[0][0] == 5);

    auto diff = A - B;
    assert(diff[0][0] == -1);

    auto product = A * B;
    // A = [[2,2],[2,2]], B = [[3,3],[3,3]] -> произведение = [[12,12],[12,12]]
    assert(product[0][0] == 12);
    assert(product[0][1] == 12);
    assert(product[1][0] == 12);
    assert(product[1][1] == 12);

    auto scalar_mult = A * 5;
    assert(scalar_mult[0][0] == 10);

    std::cout << "OK\n\n";
}

void test_identity() {
    std::cout << "Тестирование единичной матрицы...\n";

    auto I = Matrix<int>::identity(3);
    assert(I[0][0] == 1);
    assert(I[0][1] == 0);
    assert(I[1][1] == 1);
    assert(I[2][2] == 1);

    Matrix<int> A(3, 3, 5);
    auto result = A * I;
    assert(result == A);

    std::cout << "OK\n\n";
}

void test_transpose() {
    std::cout << "Тестирование транспонирования...\n";

    Matrix<int> A(2, 3);
    A[0][0] = 1; A[0][1] = 2; A[0][2] = 3;
    A[1][0] = 4; A[1][1] = 5; A[1][2] = 6;

    auto AT = A.transpose();
    assert(AT.get_rows() == 3);
    assert(AT.get_cols() == 2);
    assert(AT[0][0] == 1);
    assert(AT[0][1] == 4);
    assert(AT[1][0] == 2);
    assert(AT[1][1] == 5);
    assert(AT[2][0] == 3);
    assert(AT[2][1] == 6);

    std::cout << "OK\n\n";
}

void test_comparison() {
    std::cout << "Тестирование операторов сравнения...\n";

    Matrix<int> A(2, 2, 5);
    Matrix<int> B(2, 2, 5);
    Matrix<int> C(2, 2, 6);
    Matrix<int> D(3, 3, 5);

    assert(A == B);
    assert(A != C);
    assert(A != D);

    std::cout << "OK\n\n";
}

void run_all_tests() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     Лабораторная работа 3: Шаблон класса Matrix              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    test_constructor();
    test_copy_move();
    test_access();
    test_arithmetic();
    test_identity();
    test_transpose();
    test_comparison();

    std::cout << "=== Все тесты пройдены успешно! ===\n\n";
}

void demonstration() {
    std::cout << "=== Демонстрация работы с классом Matrix ===\n\n";

    // Создание матриц разных типов
    Matrix<int> m1(3, 3, 5);
    std::cout << "Матрица m1 (int):\n" << m1 << std::endl;

    // Доступ к элементам
    m1[0][0] = 10;
    m1.at(1, 1) = 20;
    std::cout << "После изменения элементов:\n" << m1 << std::endl;

    // Создание единичной матрицы
    auto identity = Matrix<int>::identity(3);
    std::cout << "Единичная матрица 3x3:\n" << identity << std::endl;

    // Сложение матриц
    Matrix<int> m2(3, 3, 2);
    Matrix<int> m3(3, 3, 3);
    auto sum = m2 + m3;
    std::cout << "Сумма матриц m2 и m3:\n" << sum << std::endl;

    // Транспонирование
    Matrix<int> A(2, 3);
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 3; ++j)
            A[i][j] = i + j;

    std::cout << "Матрица A:\n" << A << std::endl;
    std::cout << "Транспонированная A:\n" << A.transpose() << std::endl;

    // Скалярные операции
    Matrix<int> B(2, 2, 2);
    std::cout << "Матрица B:\n" << B << std::endl;
    std::cout << "B * 3:\n" << (B * 3) << std::endl;
    std::cout << "5 * B:\n" << (5 * B) << std::endl;
}

// ==================== main ====================
int main() {
    // Настройка русской локали
    setup_russian_locale();

    try {
        run_all_tests();
        demonstration();

        // Демонстрация работы с разными типами
        std::cout << "\n=== Работа с матрицей типа double ===\n";
        Matrix<double> dmat(2, 2, 1.5);
        std::cout << dmat << std::endl;
        std::cout << "Умножение на 2: " << (dmat * 2) << std::endl;

        // Демонстрация работы с типом float
        std::cout << "\n=== Работа с матрицей типа float ===\n";
        Matrix<float> fmat(3, 3, 1.1f);
        std::cout << fmat << std::endl;

        // Демонстрация работы с пустой матрицей
        std::cout << "\n=== Работа с пустой матрицей ===\n";
        Matrix<double> empty;
        std::cout << "Пустая матрица создана. is_empty() = "
                  << (empty.is_empty() ? "true" : "false") << std::endl;

        // Попытка создания матрицы с неверным типом (будет ошибка компиляции)
        // Matrix<std::string> smat;  // Ошибка: std::string не арифметический тип

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.get();

    return 0;
}
