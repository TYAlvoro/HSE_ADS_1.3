#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>
#include <tuple>
#include <string>

using namespace std;

// Генератор тестовых данных
class ArrayGenerator {
public:
    static vector<int> generateRandomArray(int size, int range_min, int range_max) {
        vector<int> arr(size);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(range_min, range_max);
        for (int& num : arr) {
            num = dis(gen);
        }
        return arr;
    }

    static vector<int> generateReverseArray(int size) {
        vector<int> arr(size);
        for (int i = 0; i < size; ++i) {
            arr[i] = size - i;
        }
        return arr;
    }

    static vector<int> generateNearlySortedArray(int size, int swaps) {
        vector<int> arr(size);
        for (int i = 0; i < size; ++i) {
            arr[i] = i;
        }
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, size - 1);
        for (int i = 0; i < swaps; ++i) {
            int idx1 = dis(gen);
            int idx2 = dis(gen);
            swap(arr[idx1], arr[idx2]);
        }
        return arr;
    }
};

// Реализация стандартного QUICK SORT
class QuickSort {
public:
    static void quickSort(vector<int>& arr, int left, int right) {
        if (left >= right) return;
        int pivot = partition(arr, left, right);
        quickSort(arr, left, pivot - 1);
        quickSort(arr, pivot + 1, right);
    }

private:
    static int partition(vector<int>& arr, int left, int right) {
        int pivot = arr[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (arr[j] <= pivot) {
                swap(arr[i], arr[j]);
                ++i;
            }
        }
        swap(arr[i], arr[right]);
        return i;
    }
};

// Реализация гибридного QUICK+HEAP+INSERTION SORT
class HybridSort {
public:
    static void hybridSort(vector<int>& arr, int left, int right, int depth_limit) {
        if (left >= right) return;

        if (right - left + 1 < 16) {
            insertionSort(arr, left, right);
            return;
        }

        if (depth_limit <= 0) {
            heapSort(arr, left, right);
            return;
        }

        int pivot = partition(arr, left, right);
        hybridSort(arr, left, pivot - 1, depth_limit - 1);
        hybridSort(arr, pivot + 1, right, depth_limit - 1);
    }

private:
    static void insertionSort(vector<int>& arr, int left, int right) {
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

    static void heapSort(vector<int>& arr, int left, int right) {
        int n = right - left + 1;
        for (int i = n / 2 - 1; i >= 0; --i)
            heapify(arr, n, i, left);
        for (int i = n - 1; i > 0; --i) {
            swap(arr[left], arr[left + i]);
            heapify(arr, i, 0, left);
        }
    }

    static void heapify(vector<int>& arr, int n, int i, int left) {
        int largest = i;
        int left_child = 2 * i + 1;
        int right_child = 2 * i + 2;

        if (left_child < n && arr[left + left_child] > arr[left + largest])
            largest = left_child;
        if (right_child < n && arr[left + right_child] > arr[left + largest])
            largest = right_child;

        if (largest != i) {
            swap(arr[left + i], arr[left + largest]);
            heapify(arr, n, largest, left);
        }
    }

    static int partition(vector<int>& arr, int left, int right) {
        int pivot = arr[right];
        int i = left;
        for (int j = left; j < right; ++j) {
            if (arr[j] <= pivot) {
                swap(arr[i], arr[j]);
                ++i;
            }
        }
        swap(arr[i], arr[right]);
        return i;
    }
};

// Замеры времени и сохранение результатов
class SortTester {
public:
    template <typename SortFunction>
    static long long measureTime(vector<int>& arr, SortFunction sortFunc, int left, int right) {
        auto start = chrono::high_resolution_clock::now();
        sortFunc(arr, left, right);
        auto end = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::milliseconds>(end - start).count();
    }

    static void saveResultsToCSV(const string& filename, const vector<tuple<string, int, long long>>& results) {
        ofstream file(filename);
        file << "ArrayType,ArraySize,ExecutionTime(ms)\n";
        for (const auto& result : results) {
            file << get<0>(result) << "," << get<1>(result) << "," << get<2>(result) << "\n";
        }
        file.close();
    }
};

int main() {
    vector<int> sizes = {500, 1000, 2000, 5000, 10000};
    vector<string> types = {"Random", "Reverse", "NearlySorted"};
    int swaps = 10;

    vector<tuple<string, int, long long>> results;

    for (const auto& type : types) {
        for (const auto& size : sizes) {
            vector<int> arr;
            if (type == "Random") {
                arr = ArrayGenerator::generateRandomArray(size, 0, 6000);
            } else if (type == "Reverse") {
                arr = ArrayGenerator::generateReverseArray(size);
            } else if (type == "NearlySorted") {
                arr = ArrayGenerator::generateNearlySortedArray(size, swaps);
            }

            // Замер времени для стандартного QuickSort
            auto arr_copy = arr;
            long long time_quick = SortTester::measureTime(arr_copy, QuickSort::quickSort, 0, arr.size() - 1);
            results.emplace_back(type + "_QuickSort", size, time_quick);

            // Замер времени для HybridSort
            arr_copy = arr;
            int depth_limit = 2 * static_cast<int>(log2(size));
            long long time_hybrid = SortTester::measureTime(
                arr_copy,
                [&depth_limit](vector<int>& a, int l, int r) { HybridSort::hybridSort(a, l, r, depth_limit); },
                0, arr.size() - 1);
            results.emplace_back(type + "_HybridSort", size, time_hybrid);
        }
    }

    SortTester::saveResultsToCSV("sorting_results.csv", results);

    return 0;
}
