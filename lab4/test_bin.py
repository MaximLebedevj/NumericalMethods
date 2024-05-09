import numpy as np
from matplotlib import cm
import matplotlib.pyplot as plt
import requests
import os
import struct

TOKEN = "7080239827:AAE-4Az5vD-DWrQRgm5o58sEWIDepwAkufI"
chat_id = "1376554020"
message = "The program: " + os.getcwd() + " has been completed"
url = f"https://api.telegram.org/bot{TOKEN}/sendMessage?chat_id={chat_id}&text={message}"
requests.get(url).json()

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
# TODO convert to 2D array
f = open("curr.txt", mode="rb")
ans = np.array(struct.unpack(('d'* N * M), f.read(8 * N * M)))
f.close()

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
plt.show()



