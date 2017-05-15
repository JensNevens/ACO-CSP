import pandas as pd
import os
import sys

def load_instances():
    instances = []
    for instance in os.listdir('./../data/instances'):
        if instance.endswith('.csp'):
            instance_name = instance.split('.')[0]
            instances.append(instance_name)
    return instances

def create_summary(mode):
    # Load instance filenames
    instances = load_instances()
    # Create dataframe
    if mode == '-a':
        iterables = [['as', 'mmas', 'acs'], ['best', 'worst', 'mean', 'std']]
    elif mode == '-ls':
        iterables = [['mmas', 'mmas+ls', 'mmas+ls+'], ['best', 'worst', 'mean', 'std']]
    columns = pd.MultiIndex.from_product(iterables, names=['algorithm', 'measure'])
    df = pd.DataFrame(index=instances, columns=columns)
    # Read the files
    # Get the values
    # Put them in the dataframe
    for instance in instances:
        if mode == '-a':
            instance_file = './../output/algorithms/' + instance + '.txt'
        elif mode == '-ls':
            instance_file = './../output/localsearch/' + instance + '.txt'
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
        if mode == '-a':
            data.to_csv('./../output/algorithms/' + algorithm + '-results.csv')
        elif mode == '-ls':
            data.to_csv('./../output/localsearch/' + algorithm + '-results.csv')

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        print("Provide a mode (-a or -ls)")
    else:
        mode = args[1]
        create_summary(mode)
