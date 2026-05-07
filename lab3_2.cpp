#include <iostream>
#include <cmath>
using namespace std;

// Базовый класс "Вектор на плоскости"
class Vector2D {
protected:
    double x, y; // координаты конца вектора (начало в 0,0)

public:
    // Конструктор
    Vector2D(double x, double y) : x(x), y(y) {}

    // Вычисление длины вектора
    double length() const {
        return sqrt(x * x + y * y);
    }

    // Печать полей и длины (НЕ виртуальная для статики, потом сделаем virtual для динамики)
    void print() const {
        cout << "2D Vector: (" << x << ", " << y << ")" << endl;
        cout << "Length: " << length() << endl;
    }

    // Виртуальная версия для демонстрации динамического полиморфизма
    virtual void printVirtual() const {
        cout << "2D Vector: (" << x << ", " << y << ")" << endl;
        cout << "Length: " << length() << endl;
    }

    virtual ~Vector2D() {} // виртуальный деструктор для корректного удаления
};

// Производный класс "Вектор в трехмерном пространстве"
class Vector3D : public Vector2D {
private:
    double z; // третья координата

public:
    // Конструктор с явным вызовом конструктора базового класса
    Vector3D(double x, double y, double z) : Vector2D(x, y), z(z) {}

    // Переопределённая функция длины (учитывает z)
    double length() const {
        return sqrt(x * x + y * y + z * z);
    }

    // Переопределённая функция печати (НЕ виртуальная)
    void print() const {
        Vector2D::print(); // вызов метода базового класса
        cout << "But this is 3D vector with z = " << z << endl;
    }

    // Переопределённая виртуальная функция печати
    void printVirtual() const override {
        Vector2D::printVirtual();
        cout << "3D extension: z = " << z << ", real length = " << length() << endl;
    }
};

int main() {
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif
    // Создание объектов
    Vector2D v2(3, 4);
    Vector3D v3(3, 4, 12);

    cout << "===== Обычные методы (НЕ виртуальные) — статический полиморфизм =====" << endl;
    Vector2D* ptr;

    ptr = &v2;
    ptr->print();   // Вызовется Vector2D::print()

    ptr = &v3;
    ptr->print();   // Вызовется Vector2D::print(), т.к. тип указателя — Vector2D*

    cout << "\n===== Виртуальные методы — динамический полиморфизм =====" << endl;
    ptr = &v2;
    ptr->printVirtual();   // Vector2D::printVirtual()

    ptr = &v3;
    ptr->printVirtual();   // Vector3D::printVirtual(), хотя тип указателя Vector2D*

    // Прямой вызов через объекты (без указателей) — тоже статическое связывание
    cout << "\n===== Прямой вызов через объекты =====" << endl;
    v2.print();
    v3.print();

    return 0;
}
