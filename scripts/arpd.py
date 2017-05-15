import pandas as pd
import os
import numpy as np

# Get all filenames
instances = []
for instance in os.listdir('./../data/instances'):
    if instance.endswith('.csp'):
        instance_name = instance.split('.')[0]
        instance_name = 'csp' + instance_name
        instances.append(instance_name)

instances.append('average')

# Create dataframe
iterables = [['as', 'mmas', 'acs'], list(range(1,11)) + ['arpd']]
columns = pd.MultiIndex.from_product(iterables, names=['algorithm', 'rpd'])
df = pd.DataFrame(index=instances, columns=columns)

# Read the optimal solutions file
opt = pd.read_csv('./../data/instances_opt.txt', sep=':', index_col=0)

# Read the files
# Get the values
# Put them in the dataframe
for instance in instances:
    if instance != 'average':
        instance_file = './../output/' + instance + '.txt'
        data = pd.read_csv(instance_file, sep=':', index_col=0)
        for algorithm in data:
            rpds = []
            for row in data.loc[:, algorithm].iteritems():
                trial, val = row
                ub = opt.loc[instance, 'ub']
                rpd = ((val - ub) * 100.0) / float(ub)
                rpds.append(rpd)
                df.loc[instance, (algorithm, trial)] = rpd
            arpd = np.mean(rpds)
            df.loc[instance, (algorithm, 'arpd')] = arpd

# Calculate AARPD
for algorithm in df.columns.levels[0]:
    data = df.loc[:, (algorithm, 'arpd')]
    aarpd = data.mean()
    df.loc['average', (algorithm, 'arpd')] = aarpd

# Output the files
for algorithm in df.columns.levels[0]:
    data = df.loc[:, algorithm]
    data.to_csv('./../output/' + algorithm + '-rpd.csv')
