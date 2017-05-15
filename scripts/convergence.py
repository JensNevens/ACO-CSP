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
seed = 666
for instance in instances:
    df = pd.DataFrame(columns=['as', 'mmas', 'acs'], index=list(range(0,1000)))
    for algo in ['as', 'mmas', 'acs']:
        conv_file = './../output/convergence/' + instance + '/out-' + algo + '-s' + str(seed) + '.txt'
        data = pd.read_csv(conv_file, sep=':', skipfooter=1)
        for idx, row in data.iterrows():
            budget = row['budget']
            quality = row['quality']
            df.loc[budget, algo] = quality
    df = df.fillna(method='pad')
    fig, ax = plt.subplots()
    df.plot(ax=ax)
    ax.set_xlabel('Evaluations')
    ax.set_ylabel('Solution Quality')
    fig.savefig('./../output/plots/conv-' + instance + '.pdf')
