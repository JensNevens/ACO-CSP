import pandas as pd
import os

# Get all filenames
instances = []
for instance in os.listdir('./../data/instances'):
    if instance.endswith('.csp'):
        instance_name = instance.split('.')[0]
        instance_name = 'csp' + instance_name
        instances.append(instance_name)

# Create dataframe
iterables = [['as', 'mmas', 'acs'], ['best', 'worst', 'mean', 'std']]
columns = pd.MultiIndex.from_product(iterables, names=['algorithm', 'measure'])
df = pd.DataFrame(index=instances, columns=columns)

# Read the files
# Get the values
# Put them in the dataframe
for instance in instances:
    instance_file = './../output/' + instance + '.txt'
    data = pd.read_csv(instance_file, sep=':', index_col=0)
    for algorithm in data:
        best = data.loc[:, algorithm].min()
        worst = data.loc[:, algorithm].max()
        mean = data.loc[:, algorithm].mean()
        std = data.loc[:, algorithm].std()
        df.loc[instance, (algorithm, 'best')] = best
        df.loc[instance, (algorithm, 'worst')] = worst
        df.loc[instance, (algorithm, 'mean')] = mean
        df.loc[instance, (algorithm, 'std')] = std

# Write the files
for algorithm in df.columns.levels[0]:
    data = df.loc[:, algorithm]
    data.to_csv('./../output/' + algorithm + '-results.csv')
