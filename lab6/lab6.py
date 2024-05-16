import matplotlib.pyplot as plt
import numpy as np
import math
import sympy as sp


def p(x):
    return sp.exp(3 * sp.log(sp.Abs(x + 1)) / 2)


def q(x):
    return 0


def f(x):
    return sp.exp(3 * sp.log(sp.Abs(x + 1)) / 2) * 2 / sp.sqrt(x + 1)


def phi0(x):
    return (1 / sp.sqrt(2)) * x ** 2 + 1 / 3


def phi0_df(x):
    return sp.sqrt(2) * x


def phik(k, x):
    return (1 / 2 * k) * x ** (2 * k) - x - 1 / 3


def phik_df(k, x):
    return x ** (2 * k - 1) - 1


def u(ci, x_):
    sigma = 0.0
    for i in range(len(ci)):
        sigma += ci[i] * (phik.subs({k: i + 1, x: x_}).evalf())
    return (phi0.subs(x, x_).evalf() + sigma).evalf()


def exact(x_):
    return 2 / 3 * (x_ + 1)**(3 / 2)


a = 0
b = 1

print(phi0(5).evalf())

N = 10
xi = np.linspace(a, b, N + 1)
print("xi = ", xi)

C = sp.IndexedBase('C')
j = sp.symbols('j', cls=sp.Idx)

x = sp.Symbol('x')
k = sp.Symbol('k')

n = 2  # TODO

p = p(sp.Symbol('x'))
print(p)
print(p.subs(x, 1))

q = q(sp.Symbol('x'))

f = f(sp.Symbol('x'))

phi0 = phi0(sp.Symbol('x'))
print("phi0 = ", phi0)

phi0_df = phi0_df(sp.Symbol('x'))
print("phi0_df = ", phi0_df)

phik = phik(sp.Symbol('k'), sp.Symbol('x'))
print("phik = ", phik)

phik_df = phik_df(sp.Symbol('k'), sp.Symbol('x'))
print("phik_df = ", phik_df)

# Задаем количество C_k
C_k = [C[j] for j in range(0, n)]
print(C_k)

sigma1 = C_k[0] * phik_df.subs(k, 1) + C_k[1] * phik_df.subs(k, 2)  # TODO
print("sigma1 = ", sigma1)

sigma2 = C_k[0] * phik.subs(k, 1) + C_k[1] * phik.subs(k, 2)  # TODO
print("sigma2 = ", sigma2)

systemCi = []
for i in range(1, n + 1):
    systemCi.append(sp.simplify(sp.integrate(2 * p * (phi0_df + sigma1) * phik_df.subs(k, i) -
                                             2 * q * (phi0 + sigma2) * phik.subs(k, 1) + 2 * f * phik.subs(k, i),
                                             (x, a, b))).evalf())

for i in range(len(systemCi)):
    print(systemCi[i])

system = []
for i in range(len(systemCi)):
    system.append(systemCi[i])
print(system)

C_linsolve = sp.linsolve(system, C_k[0], C_k[1])  # TODO
C = list(C_linsolve)[0]

print(C)
u_numeric = []
for i in range(N + 1):
    u_numeric.append(u(C, xi[i]))
print(u_numeric)

u_exact = []
for i in range(N + 1):
    u_exact.append(exact(xi[i]))


plt.figure(figsize=(15, 7))
sp = plt.subplot(121)

plt.plot(xi, u_numeric, 'k')
plt.plot(xi, u_exact, 'red')
plt.grid(True)
plt.show()


