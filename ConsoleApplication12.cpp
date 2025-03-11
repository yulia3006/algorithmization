#include <iostream>
#include <cmath>
#include <locale>
using namespace std;

// Метод Симпсона для численного интегрирования
double simpson_method(double (*f)(double), double a, double b, int n) {
    // Проверяем, что n чётное
    if (n % 2 == 1) {
        n += 1;  // Увеличиваем n, если оно нечётное
    }

    // Вычисляем шаг
    double delta_x = (b - a) / n;

    // Вычисление значений функции в узловых точках
    double integral = f(a) + f(b);

    // Суммируем для нечётных и чётных индексов
    for (int i = 1; i < n; i += 2) {
        integral += 4 * f(a + i * delta_x);
    }
    for (int i = 2; i < n - 1; i += 2) {
        integral += 2 * f(a + i * delta_x);
    }

    // Умножаем на шаг / 3
    integral *= delta_x / 3;
    return integral;
}


double func1(double x) {
    return x*x+4*x;
}
double func2(double x) {
    return (-x*x)-4*x;
}

int main() {
    setlocale(LC_ALL, "Russian");
    double a = 3;  // Начало интервала
    double b = 4;  // Конец интервала
    int n = 828888;   // Количество разбиений (должно быть чётным)

    double result1 = simpson_method(func1, a, b, n);
    double result2 = simpson_method(func2, a, b, n);
    double result = result1 - result2;
    cout << "Численное значение интеграла 1: " << result1 << endl;
    cout << "Численное значение интеграла 2: " << result2 << endl;
    cout << "Численное значение интеграла: " << result << endl;
    return 0;
}