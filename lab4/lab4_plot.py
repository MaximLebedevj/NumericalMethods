import numpy as np
from matplotlib import cm
import matplotlib.pyplot as plt
import requests
import os


TOKEN = "7080239827:AAE-4Az5vD-DWrQRgm5o58sEWIDepwAkufI" 
chat_id = "1376554020"
message = "The program: " + os.getcwd() + " has been completed" 
url = f"https://api.telegram.org/bot{TOKEN}/sendMessage?chat_id={chat_id}&text={message}"
requests.get(url).json()


with open('lab4_points.txt') as f:
    arrays = [[float(x) for x in line.split()] for line in f]

xi = np.array(arrays[0])
yi = np.array(arrays[0])
del arrays[0]
del arrays[0]
u1 = np.array(arrays)

cmap = plt.colormaps['magma']
X, Y = np.meshgrid(xi, yi)
fig = plt.figure(figsize=(25, 25))
ax = fig.add_subplot(121)
surf = ax.contourf(X, Y, u1.T, cmap=cm.magma)
fig.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)

cmap = plt.colormaps['magma']
X, Y = np.meshgrid(xi, yi)
ax = fig.add_subplot(122, projection='3d')
surf = ax.plot_surface(X, Y, u1.T, cmap=cm.magma, alpha=1, label='Численное решение')
fig.colorbar(surf, shrink=1, aspect=20)
plt.grid(True)
plt.show()

