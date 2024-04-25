import matplotlib.pyplot as plt
import math


def func(x_, y_):
    return x_ ** 2 - 2 * y_


def sol(x_):
    return 3 / 4 * math.exp(-2 * x_) + 1 / 2 * x_ ** 2 - 1 / 2 * x_ + 1 / 4


a, b = 0, 1
n = 10
h = (b - a) / n

x = [a]
x_Euler_M = [a + h / 2]

y_Euler = [1]
y_Euler_M = [1]
y_RK = [1]

y_exact = [sol(a)]

for i in range(1, n + 1):
    x.append(a + i * h)
    x_Euler_M.append(x[i] + h / 2)

    y_Euler.append(y_Euler[i - 1] + h * func(x[i - 1], y_Euler[i - 1]))

    y_Euler_M.append(y_Euler_M[i - 1] + h / 2 * (func(x_Euler_M[i - 1], y_Euler_M[i - 1]) + func(x_Euler_M[i], y_Euler_M[
        i - 1] + h * func(x_Euler_M[i - 1], y_Euler_M[i - 1]))))

    k0 = func(x[i - 1], y_RK[i - 1])
    k1 = func(x[i - 1] + h / 2, y_RK[i - 1] + h * k0 / 2)
    k2 = func(x[i - 1] + h / 2, y_RK[i - 1] + h * k1 / 2)
    k3 = func(x[i - 1] + h, y_RK[i - 1] + h * k2)
    y_RK.append(y_RK[i - 1] + h * (k0 + 2 * k1 + 2 * k2 + k3) / 6)

    y_exact.append(sol(x[i]))


error_Euler = []
error_Euler_M = []
error_RK = []
for i in range(n + 1):
    error_Euler.append(math.fabs(y_exact[i] - y_Euler[i]))
    error_Euler_M.append(math.fabs(y_exact[i] - y_Euler_M[i]))
    error_RK.append(math.fabs(y_exact[i] - y_RK[i]))


print(max(error_Euler))
print(max(error_Euler_M))
print(max(error_RK))


plt.figure(figsize=(15, 7))
sp = plt.subplot(121)
plt.title("Решение y' = x**2 - 2 * y")

plt.plot(x, y_Euler, 'k')
plt.plot(x, y_Euler, 'ro', label="Эйлер")

plt.plot(x_Euler_M, y_Euler_M, 'k')
plt.plot(x_Euler_M, y_Euler_M, 'bo', label="Эйлер Модификация")

plt.plot(x, y_RK, 'k')
plt.plot(x, y_RK, 'yo', label="Рунге — Кутты")

plt.plot(x, y_exact, 'k')
plt.plot(x, y_exact, 'go', label="Точное")

plt.legend()
plt.grid(True)


sp = plt.subplot(122)
plt.title("Погрешность")

plt.plot(x, error_Euler, 'k')
plt.plot(x, error_Euler, 'ro', label="Эйлер")

plt.plot(x_Euler_M, error_Euler_M, 'k')
plt.plot(x_Euler_M, error_Euler_M, 'bo', label="Эйлер Модификация")

plt.plot(x, error_RK, 'k')
plt.plot(x, error_RK, 'yo', label="Рунге — Кутты")

plt.legend()
plt.grid(True)
plt.show()
