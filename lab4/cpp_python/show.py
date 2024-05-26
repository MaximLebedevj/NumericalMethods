import matplotlib.pyplot as plt
import pickle
import sys


with open('%s' %sys.argv[1], 'rb') as file: figx = pickle.load(file)
plt.show()
