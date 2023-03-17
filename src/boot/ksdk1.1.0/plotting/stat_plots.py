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

elif type == "bar":
    x = ['-1', '0', '1', '2', '3']
    probs = [1, 1, 1, 31, 46]
    x_pos = [i for i, _ in enumerate(x)]
    plt.bar(x, probs, color='black')
    plt.xlabel("Floor level")
    plt.ylabel("Likelihood of each floor")
    plt.title("3 floor model lift movement likelihoods")
    # plt.yticks(range(0, 19000, 2000))
    plt.show()
