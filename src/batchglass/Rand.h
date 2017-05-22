// Randomness related routines
// Each instance is a state machine maintaining an independent random state

#ifndef _RAND_H
#define _RAND_H
#define NTAB 32

class Rand {
public:
    explicit Rand(int seed);
    const int seed;
   
    // Ran2 from Numerical Recipes
    double ran2();

    // Uniform random double in [a, b)
    double randu(double a, double b);

    // Random integer in [0, n)
    int randi(int n);

    // Random integer in [a, b)
    int randi(int a, int b);

    // Shuffle an int array randomly
    void shuffle(int *arr, int n);

    void test();

    void reset(); // Reset to initial random state

private:
    long ran2_idum2;
    long ran2_iy;
    long ran2_iv[NTAB];
    long ran2_idum;

};

inline double Rand::randu(double a, double b) {
    return a + ran2() * (b - a);
}

inline int Rand::randi(int n) {
    return int(ran2() * n);
}

inline int Rand::randi(int a, int b) {
    return a + randi(b - a);
}

#endif
