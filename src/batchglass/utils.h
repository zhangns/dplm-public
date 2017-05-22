#ifndef _UTILS_H
#define _UTILS_H
#define IS_BATCH 1
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#define BUF 64
#define NEWLINE '\n'
#define ENDL NEWLINE << std::flush
#define PRINTLN(msg) (std::cout<<(msg)<<std::endl)
#define FOR(i, n) for(int i = 0; i < n; ++i)
#define FOR2(i, low, high) for(int i = (low); i < (high); ++i)
#define ATOI(a) atoi((a))             // string to int
#define ATOD(a) strtod((a), nullptr)  // string to double
#define AEQ(x, y) (strcmp(x,y) == 0)  // string equality

namespace math {
    const double PI = atan(1.0) * 4;

    inline int period_dist(int x, int y, int p) {
        const int d = abs(x - y);
        return (std::min)(d, p - d);
    }

    inline double roundx(double x, int prec = 2) {
        std::stringstream ss;
        ss << std::setprecision(prec) << x;
        ss >> x;
        return x;
    }

    template<class T> 
    T square(T x) {
        return x * x;
    }

    template<class T>
    T sum(const T x[], int n) {
        T Sx = 0;
        FOR(i, n)
            Sx += x[i];
        return Sx;
    }

    inline double mean(const double x[], int n) {
        return sum(x, n) / n;
    }

    inline double stddev(const double x[], int n) {
        const double x0 = mean(x, n);
        double d = 0.0;
        FOR(i, n)
            d += square(x[i] - x0);
        return sqrt(d / (n - 1));
    }

    // threshold: fraction of x such that y(x) < 0.5
    // tolerance: a = SD * tolerance
    inline void sigmoid(const double x[], double y[], int n, double threshold, double tolerance) {
        double *xx = new double[n];
        FOR(i, n) xx[i] = x[i];
        const int rank = int(n * threshold);
        std::nth_element(xx, xx + rank, xx + n);
        const double
            x0 = xx[rank] + 1,
            a = stddev(x, n) * tolerance;
        delete[] xx;
        FOR(i, n)
            y[i] = 1 / (1 + exp(-(x[i] - x0) / a));
    }

}

#endif
