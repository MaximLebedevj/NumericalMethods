import numpy as np
import matplotlib.pyplot as plt


def exact_solution(x_i, t_i):
    return np.exp(-np.pi * t_i) * np.sin(np.pi * x_i / 4)


def initial_cond(x_i):
    return np.sin(np.pi * x_i / 4)


def thomas_algorithm(u_, m_, s_=0):
    res = u_
    a_ = np.full((N + 1, N + 1), 0.0)
    b_ = np.full((N + 1), 0.0)
    for j_ in range(M):
        a_[0][0] = 1
        a_[N][N - 1] = 1
        for i_ in range(1, N):
            a_[i_][i_] = -(1 + 2 * m_)
            a_[i_][i_ - 1] = a_[i_][i_ + 1] = m_
            if s_:
                b_[i_] = -(res[i_][j_] + m_ * (res[i_ + 1][j_] - 2 * res[i_][j_] + res[i_ - 1][j_]))
            else:
                b_[i_] = -res[i_][j_]
        for i_ in range(1, N + 1):
            a_[i_][i_] -= a_[i_ - 1][i_] * a_[i_][i_ - 1] / a_[i_ - 1][i_ - 1]
            b_[i_] -= b_[i_ - 1] * a_[i_][i_ - 1] / a_[i_ - 1][i_ - 1]
            b_[N] = 0
            a_[i_][i_ - 1] = 0
        res[N][j_ + 1] = b_[N] / a_[N][N]
        for i_ in range(1, N):
            res[N - i_][j_ + 1] = (b_[N - i_] - res[N - i_ + 1][j_ + 1] * a_[N - i_][N - i_ + 1]) / a_[N - i_][N - i_]
    return res


a = np.sqrt(16 / np.pi)
M, N = 9, 10

h = 4 / N
tau = 0.1 / M

print("CHECK", tau / h ** 2)

xi = np.linspace(0, 4, N + 1)
ti = np.linspace(0, 0.1, M + 1)

# Считаем точные значения
u_exact = np.full((N + 1, M + 1), 0.0)
for i in range(N + 1):
    for j in range(M + 1):
        u_exact[i][j] = exact_solution(xi[i], ti[j])

# Явная схема
u = np.full((N + 1, M + 1), 0.0)

# u(x, 0) = initial_cond
for i in range(N + 1):
    u[i][0] = initial_cond(xi[i])

for j in range(M):
    u[0][j + 1] = 0
    for i in range(1, N + 1):
        if i < N:
            u[i][j + 1] = u[i][j] + (tau * a ** 2) / (h ** 2) * (u[i + 1][j] - 2 * u[i][j] + u[i - 1][j])
        else:
            u[N][j + 1] = 0

# Неявная схема
u2 = np.full((N + 1, M + 1), 0.0)

# u(x, 0) = initial_cond
for i in range(N + 1):
    u2[i][0] = initial_cond(xi[i])

m = tau * a ** 2 / h ** 2
u2 = thomas_algorithm(u2, m)


# Схема Кранка-Николсона
u3 = np.full((N + 1, M + 1), 0.0)

# u(x, 0) = initial_cond
for i in range(N + 1):
    u3[i][0] = initial_cond(xi[i])

m = 1/2 * tau * a ** 2 / h**2
u3 = thomas_algorithm(u3, m, 1)


#Погрешность
error1 = abs(u - u_exact)
error2 = abs(u2 - u_exact)
error3 = abs(u3 - u_exact)

print("Error2\n", error2)
print("Error3\n", error3)


# Визуализация
X, T = np.meshgrid(xi, ti)

fig1 = plt.figure(1, figsize=(25, 25))
ax = fig1.add_subplot(231, projection='3d')
ax.plot_surface(X, T, u_exact.T, color='blue', alpha=0.5, label='Точное решение')
ax.plot_surface(X, T, u.T, color='red', alpha=0.5, label='Явная схема')
plt.legend()

fig2 = plt.figure(1, figsize=(25, 15))
ax = fig2.add_subplot(232, projection='3d')
ax.plot_surface(X, T, u_exact.T, color='blue', alpha=0.5, label='Точное решение')
ax.plot_surface(X, T, u2.T, color='red', alpha=0.5, label='Неявная схема')
plt.legend()

fig3 = plt.figure(1, figsize=(25, 25))
ax = fig3.add_subplot(233, projection='3d')
ax.plot_surface(X, T, u_exact.T, color='blue', alpha=0.5, label='Точное решение')
ax.plot_surface(X, T, u3.T, color='red', alpha=0.5, label='Схема Кракка-Николсона')
plt.legend()

fig3 = plt.figure(1, figsize=(25, 25))
ax = fig3.add_subplot(234, projection='3d')
ax.plot_surface(X, T, error1.T, color='green', alpha=0.4, label='Явная схема')
plt.legend()

fig2 = plt.figure(1, figsize=(25, 25))
ax = fig2.add_subplot(235, projection='3d')
ax.plot_surface(X, T, error2.T, color='green', alpha=0.4, label='Неявная схема')
plt.legend()

fig3 = plt.figure(1, figsize=(25, 25))
ax = fig3.add_subplot(236, projection='3d')
ax.plot_surface(X, T, error3.T, color='green', alpha=0.4, label='Схема Кракка-Николсона')
plt.legend()
plt.show()
