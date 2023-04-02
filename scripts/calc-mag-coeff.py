"""_summary_ TODO

"""

import matplotlib.pyplot as plt
import numpy as np

filename="C:/Users/Andrew/Downloads/mag-data.csv"

with open(filename, "r") as f:
    data = []
    cock = [0, 0, 0]
    ma = [-10000, -10000, -10000]
    mi = [10000, 10000, 10000]
    for line in f:
        shit = [float(i) for i in line.strip().split(",")]
        for i in range(3):
            cock[i] += shit[i]
            ma[i] = max(ma[i], shit[i])
            mi[i] = min(mi[i], shit[i])
        data.append(shit)
    print(ma)
    print(mi)
    
    off = [0, 0, 0]
    for i in range(3):
        off[i] = (ma[i] + mi[i]) / 2
    print(off)
    
    
    for i in range(len(data)):
        for j in range(3):
            data[i][j] -= off[j]
            data[i][j] /= ma[j]-mi[j]
            
    size = 100
    moving_averages=[0,0,0]
    for i in range(size):
        for j in range(3):
            moving_averages[j] += data[i][j]
    
    averaged = []
    for i in range(size, len(data)):
        for j in range(3):
            moving_averages[j] += data[i][j]
            moving_averages[j] -= data[i-size][j]
        
        averaged.append([moving_averages[0] / size, moving_averages[1] / size, moving_averages[2] / size])

    
        
    # xpoints = np.array(range(len(data)))
    # ypoints = np.array([i[0] for i in data])
    # plt.plot(xpoints, ypoints)
    # ypoints = np.array([i[1] for i in data])
    # plt.plot(xpoints, ypoints)
    # ypoints = np.array([i[2] for i in data])
    # plt.plot(xpoints, ypoints)
    # plt.show()
    
    # xpoints = np.array([i[1] for i in data])
    # ypoints = np.array([i[2] for i in data])
    # plt.scatter(xpoints, ypoints)
    # plt.show()
    # xpoints = np.array(range(len(data)))
    
    # ypoints = np.array([i[0]-off[0] for i in data])
    # # plt.plot(xpoints, ypoints)
    # ypoints = np.array([i[1]-off[1] for i in data])
    # plt.plot(xpoints, ypoints)
    # ypoints = np.array([i[2]-off[2] for i in data])
    # plt.plot(xpoints, ypoints)
    # plt.show()
    # xpoints = np.array([i[1]-off[1] for i in data])
    # ypoints = np.array([i[2]-off[2] for i in data])
    # plt.scatter(xpoints, ypoints)
    # plt.show()
    
    # xpoints = np.array(range(len(data)))
    xpoints = np.array(range(len(averaged)))
    # ypoints = np.array(averaged)
    # plt.plot(xpoints, [i[0] for i in data], label="x")
    # plt.plot(xpoints, [i[1] for i in data], label="y")
    # plt.plot(xpoints, [i[2] for i in data], label="z")
    plt.plot(xpoints, [i[0] for i in averaged], label="x")
    plt.plot(xpoints, [i[1] for i in averaged], label="y")
    plt.plot(xpoints, [i[2] for i in averaged], label="z")
    # plt.plot(xpoints, ypoints, label=["x", "y", "z"])
    plt.legend()
    plt.show()
    