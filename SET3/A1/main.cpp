#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <iomanip>
#include <vector>

using namespace std;

// Проверка, находится ли точка внутри круга
bool isInsideCircle(double x, double y, double cx, double cy, double r) {
    return (x - cx) * (x - cx) + (y - cy) * (y - cy) <= r * r;
}

// Проверка, находится ли точка внутри пересечения трех кругов
bool isInsideIntersection(double x, double y, double c1x, double c1y, double r1,
                          double c2x, double c2y, double r2,
                          double c3x, double c3y, double r3) {
    return isInsideCircle(x, y, c1x, c1y, r1) &&
           isInsideCircle(x, y, c2x, c2y, r2) &&
           isInsideCircle(x, y, c3x, c3y, r3);
}

// Точное значение площади пересечения
double exactIntersectionArea() {
    return 0.5 + 0.25 * M_PI - 0.5 + 1.25 * asin(0.8) - 1;
}

// Метод Монте-Карло
double monteCarloIntersection(int N, double x_min, double x_max, double y_min, double y_max,
                               double c1x, double c1y, double r1,
                               double c2x, double c2y, double r2,
                               double c3x, double c3y, double r3) {
    int inside_count = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis_x(x_min, x_max);
    uniform_real_distribution<> dis_y(y_min, y_max);

    for (int i = 0; i < N; ++i) {
        double x = dis_x(gen);
        double y = dis_y(gen);
        if (isInsideIntersection(x, y, c1x, c1y, r1, c2x, c2y, r2, c3x, c3y, r3)) {
            ++inside_count;
        }
    }

    double rectangle_area = (x_max - x_min) * (y_max - y_min);
    return (static_cast<double>(inside_count) / N) * rectangle_area;
}

int main() {
    // Координаты кругов
    double c1x = 1.0, c1y = 1.0, r1 = 1.0;
    double c2x = 1.5, c2y = 2.0, r2 = sqrt(5) / 2.0;
    double c3x = 2.0, c3y = 1.5, r3 = sqrt(5) / 2.0;

    // Точное значение площади
    double exact_area = exactIntersectionArea();

    // Диапазоны
    vector<double> scales = {3.0, 4.0}; // Масштабы прямоугольной области
    int N_start = 100, N_end = 100000, N_step = 500;

    // Открытие файла для записи
    ofstream csv_file("monte_carlo_results.csv");
    csv_file << "N,Scale,ApproxArea,RelativeError\n";

    // Генерация данных
    for (double scale : scales) {
        double x_min = 0, x_max = scale, y_min = 0, y_max = scale;
        for (int N = N_start; N <= N_end; N += N_step) {
            double approx_area = monteCarloIntersection(N, x_min, x_max, y_min, y_max,
                                                         c1x, c1y, r1, c2x, c2y, r2, c3x, c3y, r3);
            double relative_error = abs(approx_area - exact_area) / exact_area;
            csv_file << N << "," << scale << "," << approx_area << "," << relative_error << "\n";
        }
    }

    csv_file.close();
    cout << "Данные сохранены в monte_carlo_results.csv" << endl;
    return 0;
}
