import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib import animation


def g(x, t):
    return 0


def alpha(x):
    return 0


def psi(x):
    return 0


def gamma0(t):
    return t


def gamma1(t):
    return t * (t - 1)


a = 1
L = T = 1
N = M = 100
h = L / N
tau = T / M

xi = np.linspace(0, L, N + 1)
ti = np.linspace(0, T, M + 1)

u = np.full((N + 1, M + 1), 0.0)

# Граничное условие u(x, 0) = alpha(x)
for i in range(len(xi)):
    u[i][0] = alpha(xi[i])

# Граничное условие u(0, t) = gamma0(t)
for j in range(len(ti)):
    u[0][j] = gamma0(ti[j])

# Граничное условие u(N, t) = gamma1(t)
for j in range(len(ti)):
    u[N][j] = gamma1(ti[j])


# Численный метод
for i in range(1, len(xi) - 1):
    u[i][1] = u[i][0] + tau ** 2 / 2 * (
                a ** 2 * (u[i - 1][0] - 2 * u[i][0] + u[i + 1][0]) / h ** 2 + g(xi[i], 0)) + tau * psi(xi[i])

for j in range(1, len(ti) - 1):
    for i in range(1, len(xi) - 1):
        u[i][j + 1] = 2 * u[i][j] - u[i][j - 1] + a ** 2 * tau ** 2 / h ** 2 * (
                    u[i - 1][j] - 2 * u[i][j] + u[i + 1][j]) + tau ** 2 * g(xi[i], ti[j])


# Визуализация
fig1 = plt.figure(figsize=(25, 25))
cmap = plt.colormaps['magma']
X, Y = np.meshgrid(xi, ti)
ax = fig1.add_subplot(projection='3d')
surf = ax.plot_surface(X, Y, u.T, cmap=cm.magma, alpha=1, label='Численное решение')
fig1.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)
plt.show()


i = T  # time-step
j = L  # space-step

z = u.copy()  # 2D space-time array
x = xi.copy()  # space axis

min_u = np.min(u)

fig = plt.figure()
ax = plt.axes(xlim=(0, j),
              ylim=(np.min(u) - 0.1, np.max(u) + 0.1),
              )

pts, = ax.plot([], [])


def init():
    pts.set_data([], [])
    return pts,


def animate(i):
    # Refresh the plot
    ax.set_title("Timestamp #{}".format(i + 1))
    pts.set_data(x, z[i, :])  # plotting space axis at time-step i
    return pts,


# Set animation speed and duration
frame_dt = 100.0
frames_n = z.shape[0]

# Create and save animation
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=frames_n,
                               interval=int(1000 / frame_dt),
                               blit=False
                               )
plt.show()

