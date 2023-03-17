import pandas as pd
from matplotlib import pyplot as plt

df = pd.read_csv("true-vals.csv")

type = "hist2" # "plot"

if type == "hist":
    df = pd.read_csv("true-vals.csv")
    plt.hist(df, bins=range(4000, 4070, 2), color='black')
    plt.xlabel("Raw z-axis acceleration bins")
    plt.ylabel("Frequency count")
    plt.title("Gaussian noise produced by MMA8451Q")
    #plt.xticks(range(33500, 41500, 500))
    plt.show()

elif type == "hist1":
    df = pd.read_csv("monte-vals.csv")
    plt.hist(df, bins=range(4000, 4070, 2), color='black')
    plt.xlabel("Raw model-generated z-axis acceleration bins")
    plt.ylabel("Frequency count")
    plt.title("Gaussian noise produced by Monte Carlo")
    #plt.xticks(range(33500, 41500, 500))
    plt.show()
elif type == "hist2":
    df = pd.read_csv("sample.csv")
    plt.hist(df, bins=range(1000, 9000, 200), color='black')
    plt.xlabel("Final obtained displacement bins")
    plt.ylabel("Frequency count")
    plt.title("Displacement distribution of model lift after moving up 1 floor (30cm)")
    #plt.xticks(range(33500, 41500, 500))
    plt.show()

elif type == "plot0":
    df0 = pd.read_csv("data3_line.csv", usecols=["Measurement #"])
    df1 = pd.read_csv("data3_line.csv", usecols=[" Current (mA)"])
    plt.scatter(df0, df1, color='black')
    plt.xlabel("Measurement #")
    plt.ylabel("Current (\u03bcA)")
    plt.title("Single pixel line current measurements")
    plt.yticks(range(0, 19000, 2000))
    plt.show()
elif type == "plotRes":
    df0 = pd.read_csv("data3_resistor4.csv", usecols=["Measurement #"])
    df1 = pd.read_csv("data3_resistor4.csv", usecols=[" Current (mA)"])
    print(df1)
    plt.scatter(df0, df1, color='black')
    plt.xlabel("Measurement #")
    plt.ylabel("Current (\u03bcA)")
    plt.title("150\u03A9 resistor current measurements")
    plt.yticks(range(29700, 30300, 100))
    plt.show()