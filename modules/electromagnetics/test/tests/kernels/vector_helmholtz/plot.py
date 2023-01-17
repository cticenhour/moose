import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator

filename = 'convergence_out.csv'

h_axis_label = 'Element Size (h)'
h_axis_limits = [3.90625E-05, 0.04]

error_axis_label = 'L$_2$ Error'
error_axis_limits = [1.41E-05, 0.014531554]

legend = 'Electric Field: 1.0008'

data =[]
data.append(pd.read_csv(filename, header=0))

ax1 = plt.subplot(111)
data[0].plot(ax = ax1, x = 'h', y = 'error', color = 'dodgerblue', marker = '*', markerfacecolor = 'dodgerblue', markevery = 1, label = legend, loglog = True)

ax1.set_ylabel(error_axis_label)
ax1.set_ylim(error_axis_limits)
ax1.set_xlabel(h_axis_label)
ax1.set_xlim(h_axis_limits)

ax1.tick_params(which = 'both', direction = 'in')

plt.show()
