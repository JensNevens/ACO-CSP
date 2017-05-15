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

def create_boxplots(mode):
    instances = load_instances()
    # Read the files
    # Create boxplot
    # Store figure
    for instance in instances:
        if mode == '-a':
            instance_file = './../output/algorithms/' + instance + '.txt'
        elif mode == '-ls':
            instance_file = './../output/localsearch/' + instance + '.txt'
        data = pd.read_csv(instance_file, sep=':', index_col=0)
        fig, ax = plt.subplots()
        data.boxplot(ax=ax)
        ax.set_xlabel('Algorithms')
        ax.set_ylabel('Solution Quality')
        ax.grid('off')
        if mode == '-a':
            fig_name = './../output/plots/algorithms/box-' + instance + '.pdf'
        elif mode == '-ls':
            fig_name = './../output/plots/localsearch/box-' + instance + '.pdf'
        plt.savefig(fig_name)

if __name__ == '__main__':
    args = sys.argv
    if len(args) != 2:
        print("Provide a mode (-a or -ls)")
    else:
        mode = args[1]
        create_boxplots(mode)
