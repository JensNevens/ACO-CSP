import pandas as pd
import os
import matplotlib.pyplot as plt
import sys

def load_instances():
    instances = []
    for instance in os.listdir('./../data/instances'):
        if instance.endswith('.csp'):
            instance_name = instance.split('.')[0]
            instances.append(instance_name)
    return instances

def create_convplots(mode):
    instances = load_instances()
    # Read the files
    # Create boxplot
    # Store figure
    seed = 666
    for instance in instances:
        if mode == '-a':
            cols = ['as', 'mmas', 'acs']
            names = ['as', 'mmas', 'acs']
        elif mode == '-ls':
            cols = ['mmas', 'mmas+ls', 'mmas+ls+']
            names = ['mmas', 'mmasls', 'mmaslstun']
        df = pd.DataFrame(columns=cols, index=list(range(0,1000)))
        for i, name in enumerate(names):
            if mode == '-a':
                conv_file = './../output/convergence/algorithms/' + instance + '/out-' + name + '-s' + str(seed) + '.txt'
            elif mode == '-ls':
                conv_file = './../output/convergence/localsearch/' + instance + '/out-' + name + '-s' + str(seed) + '.txt'
            data = pd.read_csv(conv_file, sep=':', skipfooter=1)
            for idx, row in data.iterrows():
                budget = row['budget']
                quality = row['quality']
                algo = cols[i]
                df.loc[budget, algo] = quality
        df = df.fillna(method='pad')
        fig, ax = plt.subplots()
        df.plot(ax=ax)
        ax.set_xlabel('Evaluations')
        ax.set_ylabel('Solution Quality')
        if mode == '-a':
            fig_name = './../output/plots/algorithms/conv-' + instance + '.pdf'
        elif mode == '-ls':
            fig_name = './../output/plots/localsearch/conv-' + instance + '.pdf'
        fig.savefig(fig_name)

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        print("Provide a mode (-a or -ls)")
    else:
        mode = args[1]
        create_convplots(mode)
