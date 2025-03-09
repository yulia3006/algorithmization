#include <iostream>
#include <queue>
#include <chrono>

// Функция измерения времени выполнения
template <typename Func, typename... Args>
auto measure_execution_time(Func func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

// Функция добавления элемента в очередь
void add_elements_to_queue(std::queue<int>& q, int count) {
    for (int i = 0; i < count; ++i) {
        q.push(i);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::queue<int> q;
    int i = 0;
    
    auto execution_time = measure_execution_time(add_elements_to_queue, q, 2500000);
    std::cout << "Время выполнения:" << execution_time << "мс" << std::endl;
    return 0;
}