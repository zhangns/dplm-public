# boltz.py
# Zhang Linghan
# Linear regression analysis for Boltzmann distribution test
import sys

def main():
    if len(sys.argv) != 3:
        print('Usage: boltz.py boltz_file T')
        return

    import numpy as np
    import scipy.stats
    import matplotlib.pyplot as plt

    T = float(sys.argv[2])
    b_hyp = -1/T
    boltz_file = sys.argv[1]
    data = np.loadtxt(boltz_file)
    shape = np.shape(data)
    assert len(shape) == 2 and shape[1] == 2

    E, W = data[:, 0], data[:, 1]
    P = W / np.sum(W)

    x = E
    y = np.log(P)
    n = len(x)
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xx = np.sum(x * x)
    sum_xy = np.sum(x * y)
    Sxx = sum_xx - sum_x ** 2 / n
    Sxy = sum_xy - sum_x * sum_y / n
    b = Sxy / Sxx
    a = (sum_y - b * sum_x) / n
    print('b = {0}'.format(b))
    print('a = {0}'.format(a))
    res = a + b * x - y
    df = n - 2
    se_b = np.sqrt((np.sum(res ** 2) / df) / Sxx)
    tval = (b_hyp - b) / se_b
    pval = 2 * (1 - scipy.stats.t.cdf(abs(tval), df))
    print('t value {0}'.format(tval))
    print('p value {0}'.format(pval))
    

    T = -1.0 / b
    print('T = {0}'.format(T))
    plt.semilogy(E, P, ',')
    plt.semilogy(E, np.exp(a + b * E), '-')
    plt.xlabel('$E$')
    plt.ylabel('$P$')
    plt.savefig('{0}.png'.format(boltz_file))
    plt.show()

if __name__ == '__main__':
    main()
