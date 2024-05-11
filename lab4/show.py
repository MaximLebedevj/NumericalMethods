import matplotlib.pyplot as plt
import pickle

with open('FigureObject.fig.pickle', 'rb') as file: figx = pickle.load(file)
plt.show()
