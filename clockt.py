import pandas as pd

data = pd.read_csv("clockresult.txt", sep='\n', header=None)

print("Mean ")
print(pd.Series.mean(data[0]))
print("\nSquare ")
print(pd.Series.std(data[0]))


