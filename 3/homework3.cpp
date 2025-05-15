// ShellSort//Radix Sort//Quick Sort
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

template <typename Func, typename... Args>
auto measure_execution_time(Func&& func, Args&&... args) {
    auto start_time = std::chrono::high_resolution_clock::now();  // Начинаем отсчет времени
    func(std::forward<Args>(args)...);  // Выполняем переданную функцию с аргументами без сохранения результата
    auto end_time = std::chrono::high_resolution_clock::now();  // Останавливаем отсчет времени
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);  // Вычисляем продолжительность в миллисекундах
    return duration.count();  // Возвращаем результат и время выполнения
}

// Функция для получения максимального значения
int getMax(vector<int> arr)
{
    int max = arr[0];
    for (int i = 1; i < arr.size(); i++)
        if (arr[i] > max)
            max = arr[i];
    return max;
}
// Shell Sort
void ShellSort(vector<int>& arr) {
    int n = arr.size();
    int step = n / 2;

    while (step > 0) {
        for (int i = step; i < n; i++) {
            int temp = arr[i];
            int j = i;
            while ((j >= step) && (arr[j - step] > temp)) {
                arr[j] = arr[j - step];
                j = j - step;
            }
            arr[j] = temp;
        }
        step = step / 2;
    }
}
// Radix Sort 

vector<int> MergeBins(vector<vector<int>> bins) {
    vector<int> mergedArr;
    for (int i = 0; i < bins.size(); i++) {
        mergedArr.insert(mergedArr.end(), bins[i].begin(), bins[i].end());
    }
    return mergedArr;
}

void SortByDigits(vector<int>& arr, int digit) {
    vector<vector<int>> bins(10);
    for (int i = 0; i < arr.size(); i++) {
        int index = (arr[i] / digit) % 10;
        bins[index].push_back(arr[i]);
    }
    arr = MergeBins(bins);
}

void RadixSort(vector<int>& arr) {
    int max_num = getMax(arr);
    int digit = 1;
    while (max_num / digit > 0) {
        SortByDigits(arr, digit);
        digit = digit * 10;
    }
}

// Quick Sort
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        // Рекурсивный вызов для сортировки двух частей
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
std::vector<int> generateRandomVector(int n, int minValue, int maxValue) {
    // Создаем генератор случайных чисел и распределение
    std::random_device rd; // Получаем случайное число от устройства
    std::mt19937 gen(rd()); // Инициализируем генератор
    std::uniform_int_distribution<> distrib(minValue, maxValue); // Установим распределение от minValue до maxValue

    std::vector<int> vec;
    vec.reserve(n); // Резервируем память под n элементов

    // Заполняем вектор случайными числами
    for (int i = 0; i < n; ++i) {
        vec.push_back(distrib(gen)); // Генерируем и добавляем случайное число
    }

    return vec;
}

int main() {
    setlocale(LC_ALL, "Russian");
    vector<int> QS = { 90, 9000, 54, 32, 67, 87, 0 };
    vector<int> RS = { 90, 9000, 54, 32, 67, 87, 0 };
    vector<int> MS = { 90, 9000, 54, 32, 67, 87, 0 };
    int n = 9000;
    int min = 10;
    int max = 10000;
    vector<int> randomNumbers = generateRandomVector(n, min, max);

    ShellSort(MS);
    RadixSort(RS);
    quickSort(QS, 0, QS.size() - 1);
    

    for (int i : MS)
        cout << i << " ";
    cout << endl;
    for (int i : RS)
        cout << i << " ";
    cout << endl;
    for (int i : QS)
        cout << i << " ";

    cout << endl;
    auto execution_time = measure_execution_time(ShellSort, randomNumbers);
    cout << "Время выполнения: " << execution_time << " мс" << std::endl;

    return 0;
}
