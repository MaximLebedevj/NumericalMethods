import matplotlib.pyplot as plt
import numpy as np
import math


N = 10
h = 1 / N

draw_dots = False

x_values = np.linspace(0, 1, N + 1)
alpha0, alpha1 = 0, 1
beta0, beta1 = 1, 0
gamma0, gamma1 = 1, 0.5


def x_i(i_) -> float:
    return i_ * h


def f_i(xi) -> float:
    return -1 * 2 / ((xi + 1) ** 3) + 1


def q_i(xi) -> float:
    return 1


def p_i(xi) -> float:
    return xi + 1


def a_i(h_, r_) -> float:
    return 1 / h_ ** 2 * (1 + math.fabs(r_) - (math.sin(math.fabs(r_))) / (math.sin(math.fabs(r_)) + 1) - r_)


def c_i(h_, r_) -> float:
    return 1 / h_ ** 2 * (1 + math.fabs(r_) - (math.sin(math.fabs(r_))) / (math.sin(math.fabs(r_)) + 1) + r_)


def b_i(i_) -> float:
    return q_i(i_) - a_i(i_) - c_i(i_)


def exact_func(x) -> float:
    return x / (x + 1)


right = [f_i(xi) for xi in x_values]
right[0] = gamma0 * 2 * h
right[N] = gamma1

r = [p_i(xi) * h / 2 for xi in x_values]

a = [a_i(h, ri) for ri in r]
a[0] = 2 * h * alpha0 - 3 * beta0
a[N] = 0

c = [c_i(h, ri) for ri in r]
c[N] = 0
c[0] = -beta0

b = [q_i(x_values[i]) - a[i] - c[i] for i in range(N + 1)]
b[N] = alpha1
b[0] = 4 * beta0

# коэффициенты
matrix = [[0 for x in range(N + 1)] for y in range(N + 1)]
matrix[0][0] = a[0]
matrix[0][1] = b[0]
matrix[0][2] = c[0]

for i in range(1, N):
    matrix[i][i-1] = a[i]
    matrix[i][i] = b[i]
    matrix[i][i+1] = c[i]
matrix[N][N-1] = a[N]
matrix[N][N] = b[N]


# Метод прогонки
for i in range(2, N + 1):
    k = c[N + 1 - i] / b[N - i + 2]
    c[N + 1 - i] = 0
    b[N + 1 - i] -= a[N - i + 2] * k
    right[N + 1 - i] -= right[N - i + 2] * k
k = c[0] / b[2]
right[0] -= right[2] * k
b[0] -= a[2] * k

k = b[0] / b[1]
right[0] -= right[1] * k
a[0] -= a[1] * k

result = [right[0] / a[0]]
for i in range(1, N + 1):
    result.append((right[i] - a[i] * result[i - 1]) / b[i])

print("result = ", result)

# Точные значения
y_exact = [exact_func(xi) for xi in x_values]
print(y_exact)

# Погрешность
error = []
for i in range(len(result)):
    error.append(math.fabs(result[i] - y_exact[i]))
print(error)

# Визуализация
plt.figure(figsize=(15, 7))
sp = plt.subplot(121)
plt.title("Решения. n = 100")
plt.plot(x_values, result, 'r', label="Полученное решение")
if draw_dots:
    plt.plot(x_values, result, 'ro')

plt.plot(x_values, y_exact, 'g--', label="Точное решение")
if draw_dots:
    plt.plot(x_values, y_exact, 'go')
plt.grid(True)
plt.legend()

error = []
for i in range(len(result)):
    error.append(math.fabs(result[i] - y_exact[i]))


sp = plt.subplot(122)
plt.title("Погрешность")
if draw_dots:
    plt.plot(x_values, error, 'ko')
plt.plot(x_values, error, 'k--')

plt.grid(True)
plt.savefig('foo.png', bbox_inches='tight')
plt.show()
