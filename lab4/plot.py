import numpy as np
from matplotlib import cm
import matplotlib.pyplot as plt
import struct
import os
import pickle


# reading N and M
f = open("size.txt", mode="rb")
size = struct.unpack(('i' * 2), f.read(4 * 2))
N = size[0]
M = size[1]
f.close()


# reading xi and yi
f = open("steps.txt", mode="rb")
xi = np.array(struct.unpack(('d'* N), f.read(8 * N)))
yi = np.array(struct.unpack(('d' * M), f.read(8 * M)))
f.close()


# reading answer
f = open("ans.txt", mode="rb")
#ans = np.array(struct.unpack(('d'* N * M), f.read(8 * N * M)))
ans = np.fromfile(f, dtype=np.float64)
ans = np.reshape(ans, (-1, M))
f.close()


# visualization
cmap = plt.colormaps['magma']
X, Y = np.meshgrid(xi, yi)
fig = plt.figure(figsize=(25, 25))
ax = fig.add_subplot(121)
surf = ax.contourf(X, Y, ans.T, cmap=cm.magma)
fig.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)

cmap = plt.colormaps['magma']
X, Y = np.meshgrid(xi, yi)
ax = fig.add_subplot(122, projection='3d')
surf = ax.plot_surface(X, Y, ans.T, cmap=cm.magma, alpha=1, label='Численное решение')
fig.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)

pickle.dump(fig, open('%s.fig.pickle' %N, 'wb'))
plt.close("all")

# deleting all created temporary files
# os.remove("size.txt")
# os.remove("steps.txt")
# os.remove("ans.txt")



