import matplotlib.pyplot as plt
import numpy as np
import sympy as sp


def p(x_):
    return 1 / 2 / (x_ + 1)


def q(x_):
    return -1


def f(x_):
    return - 1 * sp.sqrt(x_ + 1)


def exact(x_):
    return np.sqrt(x_ + 1)


def phi_i(x, x_arr, i):
    if x_arr[i - 1] <= x <= x_arr[i]:
        return (x - x_arr[i - 1]) / h
    elif x_arr[i] <= x <= x_arr[i + 1]:
        return (x_arr[i + 1] - x) / h
    else:
        return 0


def numerical(x, C, n):
    sum = 0
    for i in range(n):
        sum += C[i] * phi_i(x, X, i + 1)
    return sum


# Параметры
gamma1, gamma2 = 1, np.sqrt(2)
n, m = 5, 10
a, b = 0, 1

C = [sp.symbols('C' + str(i + 1)) for i in range(n)]
print("C: ", C)
x = sp.Symbol('x')
k = sp.Symbol('k')

f = f(sp.Symbol('x'))
q = q(sp.Symbol('x'))
p = p(sp.Symbol('x'))


# Сводим неоднородную задачу к однородной
v = gamma1 + (gamma2 - gamma1) * (x - a) / (b - a)
F = f - p * sp.diff(v, x) - q * v
print("F = ", F)


# Задаем сетку X, кол-во элементов - кол-во C + 2
h = (b - a) / (n + 1)
X = np.linspace(a, b, n + 2)
print("X: ", X)


# Задаем сетку xi, разбивка
xi = np.linspace(a, b, m + 2)
print("xi: ", xi)


# матрица коэф. и правая часть
A = np.zeros((n, n))
d = np.zeros(n)


# Составляем СЛАУ
for i in range(n):
    el = X[i - 1]
    if i == 0:
        el = X[0]
    A[i][i] = (-2 / h) + (1 / h**2) * \
                (sp.integrate(p * (x - el), (x, el, X[i])) +
                 sp.integrate(q * (x - el)**2, (x, el, X[i])) +
                 sp.integrate(p * (x - X[i+1]), (x, X[i], X[i+1])) +
                 sp.integrate(q * (x - X[i+1])**2, (x, X[i], X[i+1])))
    if i < n - 1:
        A[i][i + 1] = (1 / h) - (1 / h**2) * \
                      (sp.integrate(p * (x - X[i+1]), (x, X[i], X[i+1])) +
                       sp.integrate(q * (x - X[i]) * (x - X[i+1]), (x, X[i], X[i+1])))
    if i > 0:
        A[i][i - 1] = (1 / h) - (1 / h**2) * \
                      (sp.integrate(p * (x - el), (x, X[i-1], X[i])) +
                       sp.integrate(q * (x - el) * (x - X[i]), (x, el, X[i])))

    d[i] = (sp.integrate(F * x, (x, el, X[i])) - X[i - 1] * sp.integrate(F, (x, el, X[i])) -
            sp.integrate(F * x, (x, X[i], X[i + 1])) + X[i + 1] * sp.integrate(F, (x, X[i], X[i + 1]))) / h

print("\nA = ", A)
print("d = ", d)

A = np.transpose(A)

C = np.linalg.solve(A, d)
print("\nC = ", C)


# Численное решение
w = np.zeros(m + 2)
for i in range(m):
    w[i + 1] = numerical(xi[i + 1], C, n)

u_numerical = np.zeros(m + 2)
for i in range(m + 2):
    u_numerical[i] = w[i] + v.subs(x, xi[i])
print("\nu_numerical = ", u_numerical)


# Точное решение
u_exact = exact(xi)
print("\nu_exact: ", u_exact)


# Невязка
error = []
for i in range(len(xi)):
   error.append(abs(u_exact[i] - u_numerical[i]))
print("\nerror : ", error)
print("\nmax error: ", max(np.abs(u_numerical - u_exact)))


# Визуализация
plt.figure(figsize=(15, 7))
sp = plt.subplot(121)

plt.plot(xi, u_numerical, 'k', label="Численное")
plt.plot(xi, u_exact, 'red', label="Точное")
plt.legend()
plt.grid(True)

sp = plt.subplot(122)
plt.plot(xi, error, 'black')
plt.plot(xi, error, 'bo', label="Погрешность")

plt.legend()
plt.grid(True)
plt.show()
