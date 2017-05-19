import pandas as pd
import os
import numpy as np
import sys

def load_instances():
    instances = []
    for instance in os.listdir('./../data/instances'):
        if instance.endswith('.csp'):
            instance_name = instance.split('.')[0]
            instances.append(instance_name)
    instances.append('average')
    return instances

def calculate_arpd(mode):
    instances = load_instances()
    # Create dataframe
    if mode == '-a':
        iterables = [['as', 'mmas', 'acs'], list(range(1,11)) + ['arpd']]
    elif mode == '-ls':
        iterables = [['mmas', 'mmas+ls', 'mmas+ls+'], list(range(1,11)) + ['arpd']]
    columns = pd.MultiIndex.from_product(iterables, names=['algorithm', 'rpd'])
    df = pd.DataFrame(index=instances, columns=columns)
    df.index.name = 'instance'
    # Read the optimal solutions file
    opt = pd.read_csv('./../data/instances_opt.txt', sep=':', index_col=0)
    # Read the files
    # Get the values
    # Put them in the dataframe
    for instance in instances:
        if instance != 'average':
            if mode == '-a':
                instance_file = './../output/algorithms/' + instance + '.txt'
            elif mode == '-ls':
                instance_file = './../output/localsearch/' + instance + '.txt'
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
        if mode == '-a':
            data.to_csv('./../output/algorithms/' + algorithm + '-rpd.csv')
        elif mode == '-ls':
            data.to_csv('./../output/localsearch/' + algorithm + '-rpd.csv')

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        print("Provide a mode (-a or -ls)")
    else:
        mode = args[1]
        calculate_arpd(mode)
