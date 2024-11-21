import pandas as pd
import matplotlib.pyplot as plt

# Загрузка данных из CSV
data = pd.read_csv("sorting_results.csv")

# Разделение типов массивов
array_types = data['ArrayType'].str.split('_').str[0].unique()

# Построение графиков для каждого типа массива
for array_type in array_types:
    plt.figure(figsize=(12, 6))
    subset = data[data['ArrayType'].str.contains(array_type)]
    for algo_type in subset['ArrayType'].str.split('_').str[1].unique():
        algo_data = subset[subset['ArrayType'].str.contains(algo_type)]
        plt.plot(algo_data['ArraySize'], algo_data['ExecutionTime(ms)'], label=f"{algo_type}")
    
    plt.xlabel("Размер массива")
    plt.ylabel("Время выполнения (мс)")
    plt.title(f"Сравнение времени выполнения для {array_type} массивов")
    plt.legend()
    plt.grid()
    plt.show()

# Общий график для всех массивов и алгоритмов
plt.figure(figsize=(12, 6))
for array_type in array_types:
    subset = data[data['ArrayType'].str.contains(array_type)]
    for algo_type in subset['ArrayType'].str.split('_').str[1].unique():
        algo_data = subset[subset['ArrayType'].str.contains(algo_type)]
        plt.plot(algo_data['ArraySize'], algo_data['ExecutionTime(ms)'], label=f"{array_type} ({algo_type})")

plt.xlabel("Размер массива")
plt.ylabel("Время выполнения (мс)")
plt.title("Общее сравнение времени выполнения всех сортировок")
plt.legend()
plt.grid()
plt.show()
