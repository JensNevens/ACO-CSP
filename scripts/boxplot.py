import pandas as pd
import os
import matplotlib.pyplot as plt

# Get all filenames
instances = []
for instance in os.listdir('./../data/instances'):
    if instance.endswith('.csp'):
        instance_name = instance.split('.')[0]
        instance_name = 'csp' + instance_name
        instances.append(instance_name)

# Read the files
# Create boxplot
# Store figure
for instance in instances:
    instance_file = './../output/' + instance + '.txt'
    data = pd.read_csv(instance_file, sep=':', index_col=0)
    fig, ax = plt.subplots()
    data.boxplot(ax=ax)
    ax.set_xlabel('Algorithms')
    ax.set_ylabel('Solution Quality')
    ax.grid('off')
    # fig.show()
    plt.savefig('./../output/plots/box-' + instance + '.pdf')
