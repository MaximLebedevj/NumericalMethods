import matplotlib.pyplot as plt
import numpy as np
import sympy as sp


def P(x_):
    return 3 / (2 * (x_ + 1))


def Q(x_):
    return 0


def F(x_):
    return 2 / sp.sqrt(x_ + 1)


def p(x_):
    x = sp.Symbol('x')
    return sp.exp(sp.integrate(P(x_), x))


def q(x):
    return p(x) * Q(x)


def f(x):
    return p(x) * F(x)


def phi0(x):
    return (1 / sp.sqrt(2)) * x ** 2 + 1 / 3


def phi0_df(x):
    return sp.sqrt(2) * x


def phik(k, x):
    return 1 / (k + 1) * x ** (k + 1) - x - 1 / 3


def phik_df(k, x):
    return x ** k - 1


def u(ci, x_):
    sigma = 0.0
    for i in range(len(ci)):
        sigma += ci[i] * (phik.subs({k: i + 1, x: x_}).evalf())
    return (phi0.subs(x, x_).evalf() + sigma).evalf()


def exact(x_):
    return 2 / 3 * (x_ + 1) ** (3 / 2)


a = 0
b = 1
N = 10

x = sp.Symbol('x')
k = sp.Symbol('k')
C = sp.IndexedBase('C')
j = sp.symbols('j', cls=sp.Idx)

f = f(sp.Symbol('x'))
q = q(sp.Symbol('x'))
p = p(sp.Symbol('x'))

xi = np.linspace(a, b, N + 1)

phi0 = phi0(sp.Symbol('x'))
phi0_df = phi0_df(sp.Symbol('x'))
phik = phik(sp.Symbol('k'), sp.Symbol('x'))
phik_df = phik_df(sp.Symbol('k'), sp.Symbol('x'))

# Задаем количество C_k
n = 2
C_k = [C[j] for j in range(0, n)]
print(C_k)

sigma_str = ""
for i in range(0, n):
    sigma_str += str(C_k[i] * phik_df.subs(k, i + 1))
    if i != n - 1:
        sigma_str += " + "
sigma1 = eval(sigma_str)
print("sigma1 = ", sigma1)

sigma_str = ""
for i in range(0, n):
    sigma_str += str(C_k[i] * phik.subs(k, i + 1))
    if i != n - 1:
        sigma_str += " + "
sigma2 = eval(sigma_str)
print("sigma2 = ", sigma2)

# Считаем значения dФ / dC_i
systemCi = []
for i in range(1, n + 1):
    systemCi.append(sp.simplify(sp.integrate(2 * p * (phi0_df + sigma1) * phik_df.subs(k, i) -
                                             2 * q * (phi0 + sigma2) * phik.subs(k, i) + 2 * f * phik.subs(k, i),
                                             (x, a, b))).evalf())

for i in range(len(systemCi)):
    print(systemCi[i])

system = []
for i in range(len(systemCi)):
    system.append(systemCi[i])
print(system)

# Решение получившейся системы
C_linsolve = sp.linsolve(system, [C_k[i] for i in range(0, n)])
C = list(C_linsolve)[0]
print("C_i values: ", C)

# Численное решение
u_numeric = []
for i in range(N + 1):
    u_numeric.append(u(C, xi[i]))
print("u_numeric: ", u_numeric)

# Точное решение
u_exact = []
for i in range(N + 1):
    u_exact.append(exact(xi[i]))
print("u_exact: ", u_exact)

# Визуализация
plt.figure(figsize=(15, 7))
sp = plt.subplot(121)

plt.plot(xi, u_numeric, 'k')
plt.plot(xi, u_exact, 'red')

plt.grid(True)
plt.show()
