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


def phi_i(i, x, h, xi):
    if x >= xi[i - 1] and x <= xi[i]:
        return (x - xi[i - 1]) / h
    elif x >= xi[i] and x <= xi[i + 1]:
        return -(x - xi[i + 1]) / h
    else:
        return 0


def numerical(ci, x_, v):
    sigma = 0.0
    for i in range(len(ci)):
        sigma += ci[i] * phi_i(i, x_, h, xi)
    return sigma + v.subs(x, x_)


# Параметры
# n - кол-во Ck
# N - ков-во разбиений

gamma1, gamma2 = 0.5, np.sqrt(2)

n = 5
a = 0
b = 1
N = 9

x = sp.Symbol('x')
k = sp.Symbol('k')
C = sp.IndexedBase('C')
j = sp.symbols('j', cls=sp.Idx)
u = sp.Symbol('u')

f = f(sp.Symbol('x'))
q = q(sp.Symbol('x'))
p = p(sp.Symbol('x'))


# Сводим неоднородную задачу к однородной
v = gamma1 + (gamma2 - gamma1) / (b - a) * (x - a)
v_df = sp.diff(v, x)

F = f - p * v_df - q * v
print("F = ", F)


# Задаем сетку xi с шагом h
xi = []
h = (b - a) / (N + 1)
print("h = ", h)

xi.append(a + 0 * h)
for i in range(1, N + 1):
    xi.append(a + i * h)
xi.append(a + (N + 1) * h)

print("xi = ", xi)


# Задаем количество C_k
C_k = [C[j] for j in range(0, n)]
print("\nC_k = ", C_k)


# Составляем СЛАУ
A = np.array([[0 for x in range(n+1)] for y in range(n)])
for i in range(n):
    for j in range(n):
        A[i][i] = (-2 / h) + (1 / h**2) * \
                (sp.integrate(p * (x - xi[i-1]), (x, xi[i-1], xi[i])) + 
                 sp.integrate(q * (x - xi[i-1])**2, (x, xi[i-1], xi[i])) + 
                 sp.integrate(p * (x - xi[i+1]), (x, xi[i], xi[i+1])) + 
                 sp.integrate(q * (x - xi[i+1])**2, (x, xi[i], xi[i+1])))

        if j == i + 1:
            A[i][j] = (1 / h) - (1 / h**2) * \
                    (sp.integrate(p * (x - xi[i+1]), (x, xi[i], xi[i+1])) + 
                     sp.integrate(q * (x - xi[i]) * (x - xi[i+1]), (x, xi[i], xi[i+1])))

        if j == i - 1:
            A[i][j] = (1 / h) - (1 / h**2) * \
                    (sp.integrate(p * (x - xi[i-1]), (x, xi[i-1], xi[i])) +
                     sp.integrate(q * (x - xi[i-1]) * (x - xi[i]), (x, xi[i-1], xi[i])))

# Правая часть
d = np.array([0 for x in range(n)])
for i in range(n):
    d[i] = (1 / h) * \
           (sp.integrate(F * (x - xi[i-1]), (x, xi[i-1], xi[i])) +
            sp.integrate(F * (x - xi[i+1]), (x, xi[i], xi[i+1]))) 

for i in range(n):
    A[i][n] = d[i]
print("A = ", A)


# Решаем СЛАУ, находим Ci
if n > 1:
    lines = []
    for i in range(n):
        lines.append(A[i])
    print("lines = ", lines)
    Ci = sp.linsolve(sp.Matrix((lines))).args[0]
else:
    Ci = []
    Ci.append(A[0][1] / A[0][0])
print("Ci = ", Ci)

# Численное решение 
u_numerical = []
for i in range(len(xi)):
    u_numerical.append(numerical(Ci, xi[i], v))
print("u_numerical = ", u_numerical)


# Точное решение
u_exact = []
for i in range(len(xi)):
    u_exact.append(exact(xi[i]))
print("\nu_exact: ", u_exact)


# Невязка
error = []
for i in range(len(xi)):
   error.append(abs(u_exact[i] - u_numerical[i]))
print("\nerror : ", error)


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
