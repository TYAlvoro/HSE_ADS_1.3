import pandas as pd
import matplotlib.pyplot as plt

# Загрузка данных
data = pd.read_csv("monte_carlo_results.csv")

# Построение графиков
scales = data['Scale'].unique()

# График приближённой площади
plt.figure(figsize=(12, 6))
for scale in scales:
    subset = data[data['Scale'] == scale]
    plt.plot(subset['N'], subset['ApproxArea'], label=f"Scale {scale}")
plt.axhline(y=data['ApproxArea'].mean(), color='red', linestyle='--', label="Exact Area")
plt.xlabel("Количество точек (N)")
plt.ylabel("Приближённая площадь")
plt.title("Изменение приближённой площади")
plt.legend()
plt.grid()
plt.show()

# График относительного отклонения
plt.figure(figsize=(12, 6))
for scale in scales:
    subset = data[data['Scale'] == scale]
    plt.plot(subset['N'], subset['RelativeError'], label=f"Scale {scale}")
plt.xlabel("Количество точек (N)")
plt.ylabel("Относительное отклонение")
plt.title("Относительное отклонение от точного значения")
plt.legend()
plt.grid()
plt.show()
