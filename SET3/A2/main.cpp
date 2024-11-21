#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

class ArrayGenerator {
public:
    static std::vector<int> generateRandomArray(int size, int range_min, int range_max) {
        std::vector<int> arr(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(range_min, range_max);
        for (int& num : arr) {
            num = dis(gen);
        }
        return arr;
    }

    static std::vector<int> generateReverseArray(int size) {
        std::vector<int> arr(size);
        for (int i = 0; i < size; ++i) {
            arr[i] = size - i;
        }
        return arr;
    }

    static std::vector<int> generateNearlySortedArray(int size, int swaps) {
        std::vector<int> arr(size);
        for (int i = 0; i < size; ++i) {
            arr[i] = i;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, size - 1);
        for (int i = 0; i < swaps; ++i) {
            int idx1 = dis(gen);
            int idx2 = dis(gen);
            std::swap(arr[idx1], arr[idx2]);
        }
        return arr;
    }
};

class SortTester {
public:
    // Merge Sort
    static void mergeSort(std::vector<int>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    static void merge(std::vector<int>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        std::vector<int> L(n1), R(n2);

        for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
        for (int i = 0; i < n2; ++i) R[i] = arr[mid + 1 + i];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }

        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    // Гибридный Merge + Insertion Sort
    static void hybridSort(std::vector<int>& arr, int left, int right, int threshold) {
        if (right - left + 1 <= threshold) {
            insertionSort(arr, left, right);
            return;
        }
        int mid = left + (right - left) / 2;
        hybridSort(arr, left, mid, threshold);
        hybridSort(arr, mid + 1, right, threshold);
        merge(arr, left, mid, right);
    }

    static void insertionSort(std::vector<int>& arr, int left, int right) {
        for (int i = left + 1; i <= right; ++i) {
            int key = arr[i];
            int j = i - 1;
            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }

    // Замер времени выполнения сортировки
    static long long measureTime(std::vector<int>& arr, void (*sortFunc)(std::vector<int>&, int, int), int left, int right) {
        auto start = std::chrono::high_resolution_clock::now();
        sortFunc(arr, left, right);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }
};

class CSVWriter {
public:
    // Сохранение результатов в файл
    static void saveResultsToCSV(const std::string& filename, const std::vector<std::tuple<std::string, int, long long>>& results) {
        std::ofstream file(filename);
        file << "ArrayType,ArraySize,ExecutionTime(ms)\n";
        for (const auto& result : results) {
            file << std::get<0>(result) << "," << std::get<1>(result) << "," << std::get<2>(result) << "\n";
        }
        file.close();
        std::cout << "Results saved to " << filename << std::endl;
    }
};

int main() {
    // Параметры эксперимента
    std::vector<int> sizes = {500, 1000, 2000, 5000, 10000};
    std::vector<std::string> types = {"Random", "Reverse", "NearlySorted"};
    int swaps = 10; // Количество перестановок в почти отсортированном массиве

    // Результаты экспериментов
    std::vector<std::tuple<std::string, int, long long>> results;

    for (const auto& type : types) {
        for (const auto& size : sizes) {
            std::vector<int> arr;
            if (type == "Random") {
                arr = ArrayGenerator::generateRandomArray(size, 0, 6000);
            } else if (type == "Reverse") {
                arr = ArrayGenerator::generateReverseArray(size);
            } else if (type == "NearlySorted") {
                arr = ArrayGenerator::generateNearlySortedArray(size, swaps);
            }

            // Замер времени для стандартного Merge Sort
            auto arr_copy = arr;
            long long time_merge = SortTester::measureTime(arr_copy, SortTester::mergeSort, 0, arr.size() - 1);
            results.emplace_back(type + "_MergeSort", size, time_merge);

            // Замер времени для гибридного Merge+Insertion Sort
            arr_copy = arr;
            long long time_hybrid = SortTester::measureTime(
                arr_copy,
                [](std::vector<int>& a, int l, int r) { SortTester::hybridSort(a, l, r, 15); },
                0, arr.size() - 1);
            results.emplace_back(type + "_HybridSort", size, time_hybrid);
        }
    }

    // Сохранение результатов
    CSVWriter::saveResultsToCSV("sorting_results.csv", results);

    return 0;
}
