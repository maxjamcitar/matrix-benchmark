#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys # for argv
import matplotlib.pyplot as plt
import matplotlib.pylab as plb
import pandas as pd
import math

filePath = sys.argv[1] # path to text file
nameImage = sys.argv[2] # name of future plot image

# 0=dimension, 1=cycles, 2=time, 3=processor
data = pd.read_table(filePath, sep='\t', header=None, names={0, 1, 2, 3})

arm_data = data.loc[data[3] == "ARMv8 Processor rev 1 (v8l)"]
intel_data = data.loc[data[3] == "Intel(R) Core(TM) i5-7200U CPU @ 2.50GHz"]

fig = plt.figure(figsize=(8.00, 6.00)) # 1920x1080 image
ax = fig.add_subplot(1, 1, 1)

for i_data in [arm_data, intel_data]:
    dimensions = i_data[0].unique() # receive all distances
    means=[]
    errors=[]
    for dim in dimensions:
        means.append((pd.Series.mean(i_data[2].loc[i_data[0] == dim])))
        errors.append((pd.Series.std(i_data[2].loc[i_data[0] == dim])))

    #ax.set_yscale('log')
    plt.xticks(dimensions)
    if i_data.equals(arm_data):
        plt.errorbar(dimensions, means, yerr=errors, marker='^', label="ARMv8 Processor rev 1 (v8l)")
    elif i_data.equals(intel_data):
        plt.errorbar(dimensions, means, yerr=errors, marker='^', label="Intel(R) Core(TM) i5-7200U CPU @ 2.50GHz")

# data['0'] = time, data['1'] = Rx(t)
#plt.plot(data[0], data[3])
#plb.ylim(17000, 18000)

plt.legend()

plt.title("GFLOPS as function of matrix dimension, 200 cycles")

plb.savefig(nameImage)
print("Graph saved in " + nameImage + ".")

file = open("matrix_means+errors.txt", "w")
i = 0
while i < dimensions.size:
    file.write (str(dimensions[i]) + '\t' + str(means[i]) + '\t' + str(errors[i]) + '\n')
    i+=1
file.close()
