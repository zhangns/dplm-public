# bonds.py
# Zhang Linghan
# Bond energy distribution
import sys

def main():
    if len(sys.argv) != 2:
        print('Usage: bonds.py bonds_file')
        return

    import numpy as np
    import scipy.stats
    import matplotlib.mlab as mlab
    import matplotlib.pyplot as plt

    bonds_file = sys.argv[1]
    E = np.loadtxt(bonds_file)
    N = len(E)
    nbins = 20
    n, bins = np.histogram(E, nbins)
    x = (bins[1:] + bins[:-1]) / 2
    slope, intercept, r_value, p_value, std_err = scipy.stats.linregress(x, np.log(n))
    print('slope, intercept, r_value, p_value, std_err')
    print([slope, intercept, r_value, p_value, std_err])
    print('T = {0}'.format(-1 / slope))
    plt.semilogy(x, n, '+');
    plt.semilogy(x, np.exp(slope * x + intercept), '-')
    plt.xlabel('Bond Energy')
    plt.ylabel('#')
    plt.title('y = exp({0:.4g} x + {1:.4g})'.format(slope, intercept))
    plt.savefig('{0}.png'.format(bonds_file))
    plt.show()

if __name__ == '__main__':
    main()
