import pandas as pd

data = pd.read_csv("clockresult.txt", sep='\n', header=None)

print("Mean ",end=' ')
print(pd.Series.mean(data[0]))
print("Square ", end=' ')
print(pd.Series.std(data[0]))
print("Min ", end=' ')
print(pd.Series.min(data[0]))
print("Max ", end=' ')
print(pd.Series.max(data[0]))


