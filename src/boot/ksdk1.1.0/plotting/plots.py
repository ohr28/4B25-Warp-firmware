import pandas as pd
from matplotlib import pyplot as plt

df = pd.read_csv("data3.csv", usecols=[" Current (mA)"])

type = "hist" # "plot"

if type == "hist":
    df = pd.read_csv("data3.csv", usecols=[" Current (mA)"])
    plt.hist(df, bins=range(32900, 41500, 100), color='black')
    plt.xlabel("Current Bins (\u03bcA)")
    plt.ylabel("Frequency Count")
    plt.title("Histogram of OLED current measurements")
    plt.xticks(range(33500, 41500, 500))
    plt.show()
elif type == "plot":
    df0 = pd.read_csv("data3.csv", usecols=["Measurement #"])
    df1 = pd.read_csv("data3.csv", usecols=[" Current (mA)"])
    plt.scatter(df0, df1, color='black')
    plt.xlabel("Measurement #")
    plt.ylabel("Current (\u03bcA)")
    plt.title("OLED current measurements")
    plt.yticks(range(33000, 42000, 1000))
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