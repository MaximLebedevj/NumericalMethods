import matplotlib.pyplot as plt
import numpy as np
import sympy as sp


def P(x_):
    return 1 / 2 / (x_ + 1)


def Q(x_):
    return -1


def F(x_):
    return - 1 * sp.sqrt(x_ + 1)


def phi0(x):
    return (sp.sqrt(2) - 1) * x + 1


def phi0_df(x):
    return (sp.sqrt(2) - 1)


def phik(k, x):
    return x**k * (1 - x)


def phik_df(k, x):
    return k * x**(k-1) * (1 - x) - x**k


def numerical(ci, x_):
    sigma = 0.0
    for i in range(len(ci)):
        sigma += ci[i] * (phik(k, x).subs({k: i + 1, x: x_}).evalf())
    return (phi0(x).subs(x, x_).evalf() + sigma).evalf()


def exact(x_):
    return np.sqrt(x_ + 1)


def equation(u):
    return sp.diff(u, x, 2) + P * sp.diff(u, x, 1) + Q * u - F


def phi_i(i, x, h, xi):
    if x >= xi[i - 1] and x <= xi[i]:
        return (x - xi[i - 1]) / h 
    elif x >= xi[i] and x <= x[i + 1]:
        return -(x - xi[i + 1]) / h
    else:
        return 0


# Параметры 
# n - кол-во Ck
# N - ков-во разбиений
n = 1
a = 0
b = 1
N = 20

x = sp.Symbol('x')
k = sp.Symbol('k')
C = sp.IndexedBase('C')
j = sp.symbols('j', cls=sp.Idx)
u = sp.Symbol('u')

F = F(sp.Symbol('x'))
Q = Q(sp.Symbol('x'))
P = P(sp.Symbol('x'))

xi = np.linspace(a, b, N + 1)
h = (b - a) / (N)
print("h = ", h) 
print("xi = ", xi) 

print("\nphi0    = ", phi0(x))
print("phi0_df = ", phi0_df(x))
print("phik    =", phik(k, x))
print("phik_df =", phik_df(k, x))

# Задаем количество C_k
#C_k = [C[j] for j in range(0, n)]
#print("\nC_k = ", C_k)

#sigma_str = ""
#for i in range(0, n):
    #    sigma_str += str(C_k[i] * phik(k, x).subs(k, i + 1))
#    if i != n - 1:
        #        sigma_str += " + "
#sigma_phik = eval(sigma_str)
#print("sigma_phik = ", sigma_phik)  

# Решение u в виде(4)
#ux = phi0(x) + sigma_phik
#print("\nu(x) = ", ux)

# R(x, C1, ..., Cn)
#R = equation(ux)
#print("\nR(x, C1, ..., Cn) = ", R)

# СЛАУ 
#systemCi = []
#for i in range(1, n + 1):
    #    systemCi.append(sp.simplify(sp.integrate(phik(k, x).subs(k, i) * R, (x, a, b))).evalf())
#print("\nsystemCi =", systemCi)

# Решение СЛАУ
#systemCi_sol = list(sp.linsolve(systemCi, [C_k[i] for i in range(0, n)]))[0]

#print()
#for i in range(0, n):
    #    print("C{0}".format(i) ," = ", systemCi_sol[i])

# Численное решение
#u_numerical = []
#for i in range(N + 1):
    #    u_numerical.append(numerical(systemCi_sol, xi[i]))
#print("\nu_numerical: ", u_numerical)

# Точное решение
#u_exact = []
#for i in range(N + 1):
    #    u_exact.append(exact(xi[i]))
#print("\nu_exact: ", u_exact)

# Невязка
#error = []
#for i in range(N + 1):
    #    error.append(abs(u_exact[i] - u_numerical[i]))
#print("\nerror : ", error)

# Визуализация
#plt.figure(figsize=(15, 7))
#sp = plt.subplot(121)

#plt.plot(xi, u_numerical, 'k', label="Численное")
#plt.plot(xi, u_exact, 'red', label="Точное")
#plt.legend()
#plt.grid(True)

#sp = plt.subplot(122)
#plt.plot(xi, error, 'black')
#plt.plot(xi, error, 'bo', label="Погрешность")

#plt.legend()
#plt.grid(True)
#plt.show()
