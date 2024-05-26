import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib import cm


def phi(x_, y_):
    return -x_ + 4 * y_ ** 2


a = b = 1
M = N = 100
hx = a / N
hy = b / M
eps = 1e-4

xi = np.linspace(0, a, N + 1)
yi = np.linspace(0, b, M + 1)

print("xi = ", xi)
print("yi = ", yi)


u = np.full((N + 1, M + 1), 0.0)

# Граничное условие u(x, 0)
for i in range(len(xi)):
    u[i][0] = phi(xi[i], 0)

# Граничное условие u(x, M)
for i in range(len(xi)):
    u[i][M] = phi(xi[i], yi[M])

# Граничное условие u(0, y)
for j in range(len(yi)):
    u[0][j] = phi(0, yi[j])

# Граничное условие u(N, y)
for j in range(len(yi)):
    u[N][j] = phi(xi[N], yi[j])

# u0 - предыдущий, u1 - текущий
u0 = u.copy()
u1 = u.copy()

print("u = \n", u0)

# Метод Зейделя
it = 0
while True:
    it += 1
    for i in range(1, len(xi) - 1):
        for j in range(1, len(yi) - 1):
            u1[i][j] = 1 / 4 * (u1[i - 1][j] + u0[i + 1][j] + u0[i][j + 1] + u1[i][j - 1])
    if np.max(np.fabs(u1 - u0)) <= eps:
        break
    u0 = u1.copy()

print("iter = ", it)
print("max error = ", np.max(np.fabs(u1 - u0)))
print(u1)


# Визуализация
cmap = mpl.colormaps['magma']
X, Y = np.meshgrid(xi, yi)
fig1 = plt.figure(figsize=(25, 25))
ax = fig1.add_subplot(121)
surf = ax.contourf(X, Y, u1.T, cmap=cm.magma)
fig1.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)

cmap = mpl.colormaps['magma']
X, Y = np.meshgrid(xi, yi)
ax = fig1.add_subplot(122, projection='3d')
surf = ax.plot_surface(X, Y, u1.T, cmap=cm.magma, alpha=1, label='Численное решение')
fig1.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)
plt.show()
