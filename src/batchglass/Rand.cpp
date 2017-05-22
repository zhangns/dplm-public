#include <iostream>
#include <utility>
#include "Rand.h"
#include "utils.h"
using namespace std;

Rand::Rand(int seed) : seed(seed) {
    reset();
}

#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-20
#define RNMX (1.0-EPS)

double Rand::ran2() {

    int j;
    long k;
    double temp;

    if (ran2_idum <= 0) {
        if (seed != 0) ran2_idum = -seed;
        if (-(ran2_idum) < 1) ran2_idum = 1;
        else ran2_idum = -(ran2_idum);
        ran2_idum2 = (ran2_idum);
        for (j = NTAB + 7; j >= 0; j--) {
            k = (ran2_idum) / IQ1;
            ran2_idum = IA1*(ran2_idum - k*IQ1) - k*IR1;
            if (ran2_idum < 0) ran2_idum += IM1;
            if (j < NTAB) ran2_iv[j] = ran2_idum;
        }
        ran2_iy = ran2_iv[0];
    }
    k = (ran2_idum) / IQ1;
    ran2_idum = IA1*(ran2_idum - k*IQ1) - k*IR1;
    if (ran2_idum < 0) ran2_idum += IM1;
    k = ran2_idum2 / IQ2;
    ran2_idum2 = IA2*(ran2_idum2 - k*IQ2) - k*IR2;
    if (ran2_idum2 < 0) ran2_idum2 += IM2;
    j = ran2_iy / NDIV;
    ran2_iy = ran2_iv[j] - ran2_idum2;
    ran2_iv[j] = ran2_idum;
    if (ran2_iy < 1) ran2_iy += IMM1;
    if ((temp = AM*ran2_iy) > RNMX) return RNMX;
    else return temp;
}

// https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
void Rand::shuffle(int *arr, int n) {
    for (int i = n - 1; i >= 0; --i)
        swap(arr[i], arr[randi(i + 1)]);
}

void Rand::test() {
    const int M = 5, N = 10000;
    int bins[M] = {};
    FOR(i, N)
        ++bins[Rand::randi(M)];
    FOR(b, M)
        cout << b << ' ' << bins[b] << endl;

    FOR(i, N)
        ++bins[Rand::randi(1, 4)];
    FOR(b, M)
        cout << b << ' ' << bins[b] << endl;

    int arr[] = {1, 2, 3};
    FOR(t, 30) {
        Rand::shuffle(arr, 3);
        FOR(i, 3)
            cout << arr[i] << ' ';
        cout << endl;
    }
}

void Rand::reset() {
    ran2_idum2 = 123456789L;
    ran2_iy = 0L;
    FOR(i, NTAB)
        ran2_iv[i] = 0L;
    ran2_idum = -12345L;
}
