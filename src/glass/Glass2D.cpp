#include <cmath>
#include <utility>
#include "Glass2D.h"
#include "utils.h"
#include "Rand.h"

#define R(x,y) ((x)*L+(y))                // Flatten position
#define EID(v,dv) (N_DIRS*((v)-N)+(dv))   // Void hopping event id
#define FORL(x) FOR(x, L)
#define FORV(r) FOR(r, V)
#define FORN(i) FOR(i, N)
#define FORD(d) FOR(d, N_DIRS)
#define IS_PART(i) ((i) < N)
#define IS_VOID(i) ((i) >= N)
#if IS_BATCH
    #define IS_FROZEN(i) false
#else
    #define IS_FROZEN(i) ((i) < c.boltz)
#endif
#define GAMMA(i, j, xi, yi, xj, yj) (bondTable[bondPerms[R(xi, yi) * N + i] * N + bondPerms[R(xj, yj) * N + j]])

void Glass2D::_update_energy() {
    energy = 0.0;
    FORL(xi) {
        FORL(yi) {
            const int i = lattice(xi, yi);
            if (IS_PART(i)) {
                int xj, yj;
                for (int d : {RIGHT, UP}) {
                    disp(xi, yi, d, xj, yj);
                    const int j = lattice(xj, yj);
                    if (IS_PART(j))
                        energy += GAMMA(i, j, xi, yi, xj, yj);
                }
            }
        }
    }
}

void Glass2D::boltzmannize() {
    _boltzmannize();
    _update_energy();
    update_T(T);
}

void Glass2D::reset_time() {
    mcs = 0;
    time = 0.0;
}

void Glass2D::_boltzmannize() {
    // Mark uninitialized
    FORN(i)
        FOR2(j, i + 1, N)
            bondTable[i * N + j] = c.g0 - 100.0;
    // For each bond
    FORL(xi) {
        FORL(yi) {
            const int i = lattice(xi, yi);
            if (IS_PART(i)) {
                for (int d : {RIGHT, UP}) {
                    int xj, yj;
                    disp(xi, yi, d, xj, yj);
                    const int j = lattice(xj, yj);
                    if (IS_PART(j)) {
                        const double gamma = -T * log(rand.randu(exp(-(c.g0 + 1.0) / T), exp(-c.g0 / T)));
                        GAMMA(i, j, xi, yi, xj, yj) = GAMMA(j, i, xj, yj, xi, yi) = gamma;
                    }
                }
            }
        }
    }
    // Assign uniform random value to uninitialized entries
    FORN(i)
        FOR2(j, i + 1, N)
            if (bondTable[i * N + j] < c.g0)
                bondTable[i * N + j] = bondTable[j * N + i] = c.g0 + c.dg * rand.ran2();
    rand.reset();
}

void Glass2D::_loadr(const int *newX, const int *newY) {
    FORN(i) {
        X[i] = newX[i];
        Y[i] = newY[i];
    }
    FORV(r) {
        // Mark uninitialized sites
        _lattice[r] = -1;
    }
    // Particles
    FORN(i)
        lattice(X[i], Y[i]) = i;
    // Voids
    int v = N;
    FORL(x) {
        FORL(y) {
            if (lattice(x, y) == -1) {
                lattice(x, y) = v;
                X[v] = x;
                Y[v] = y;
                ++v;
            }
        }
    }
}

void Glass2D::_randr() {
    // Random initial configuration
    FORV(r) _lattice[r] = r;
    rand.shuffle(_lattice, V);
    rand.reset();
    FORL(x) {
        FORL(y) {
            const int i = lattice(x, y);
            X[i] = x; Y[i] = y;
        }
    }
}

Glass2D::~Glass2D() {
    delete[] X;
    delete[] Y;
    delete[] _lattice;
    delete[] bondTable;
    delete[] bondPerms;
}

Glass2D::Glass2D(const Config &c, Rand &rand, const int *X0, const int *Y0, const int *Xr, const int *Yr):

        L(c.L), V(L * L), N(c.N), NV(V - N), X(new int[V]), Y(new int[V]),
        rand(rand), c(c), T(c.T), _lattice(new int[V]),
        bondTable(new double[N * N]), bondPerms(new int[V * N]),
        rtree(N_DIRS * (V - N), rand) {

    // Permutations
    FORV(r) {
        FORN(i)
            bondPerms[r * N + i] = i;
        rand.shuffle(&bondPerms[r * N], N);
    }
    rand.reset();


    // Bond and configuration
    if (c.apriori == AP_OFF) {
        // Assign random values to bondTable
        FORN(i)
            FOR2(j, i + 1, N)
                bondTable[i * N + j] = bondTable[j * N + i] = c.g0 + c.dg * rand.ran2();
        rand.reset();
        if (Xr)
            _loadr(Xr, Yr);
        else
            _randr();
    } else if (c.apriori == AP_INSTANT) {
        if (Xr) {
            _loadr(X0, Y0);
            _boltzmannize();
            _loadr(Xr, Yr);
        } else {
            _randr();
            _boltzmannize();
        }
    } else {
        if (Xr) { // No need for preheat
            _loadr(X0, Y0);
            _boltzmannize();
            _loadr(Xr, Yr);
        } else { // Will do preheat
            _randr();
            // TODO: dgamma = 1 assumed
            const double u = c.g0 - T * log(T * (1 - exp(-1 / T)));
            FORN(i)
                FORN(j)
                    bondTable[i * N + j] = u;
        }
    }

    // Update energy and rates
    _update_energy();
    update_T(T);
}


// TODO
double Glass2D::cal_rate(int i, int d) const {
    if (IS_FROZEN(i)) return 0.0;
    double dE = 0.0; // Energy difference due to hopping
    //int nij = 0;     // Number of bonds
    int xv, yv;
    disp(X[i], Y[i], d, xv, yv);
    int xj, yj;
    FORD(d1) {
        if (d1 == d) continue;
        disp(X[i], Y[i], d1, xj, yj);
        const int j = lattice(xj, yj);
        if (IS_PART(j)) {
            // To break bond
            //++nij;
            dE -= GAMMA(i, j, X[i], Y[i], xj, yj);
        }
    }
    FORD(d2) {
        if (d2 == INVDIR(d)) continue;
        disp(xv, yv, d2, xj, yj);
        const int j = lattice(xj, yj);
        if (IS_PART(j)) {
            // To form bond
            //++nij;
            dE += GAMMA(i, j, xv, yv, xj, yj);
        }
    }
    //static const double w0 = 1e12, EB0 = 3.0;
    static const double w0 = 1e6, EB0 = 1.5;
    const double EB = EB0 + dE/2;
    return w0 * exp(-EB/T);
}

void Glass2D::update_T(double newT) {
    T = newT;
    // Recalculate every event rate
    // Each void, each direction
    FOR2(v, N, V) {
        int xp, yp;
        FORD(dv) {
            disp(X[v], Y[v], dv, xp, yp);
            const int p = lattice(xp, yp);
            rtree.update_rate(
                EID(v, dv),
                (IS_PART(p) && !IS_FROZEN(p)) ? cal_rate(p, INVDIR(dv)) : 0.0
            );
        }
    }
    duration = 1 / rtree.get_total_rate();
}

void Glass2D::step() {
    // Update counter and timer
    ++mcs;
    time += duration;

    const int eid = rtree.select_event(); // Let rtree decide the transition
    const int q = eid >> 2;
    hopv = q + N;                         // Void id
    const int dv = eid & 3;               // Direction of the void
    const int xv = X[hopv], yv = Y[hopv]; // Current void position
    int xp, yp;                           // Current particle position
    disp(xv, yv, dv, xp, yp);
    hopp = lattice(xp, yp);               // Particle id
    dp = INVDIR(dv);                      // Direction of the particle

    // Update coordinates
    std::swap(lattice(xv, yv), lattice(xp, yp));
    std::swap(X[hopv], X[hopp]);
    std::swap(Y[hopv], Y[hopp]);
    rtree.update_rate(EID(hopv, dp), cal_rate(hopp, dv));
    FORD(d) {
        int xj, yj, j;
        if (d != dp) {
            disp(xp, yp, d, xj, yj);
            j = lattice(xj, yj);
            if (IS_PART(j)) {
                // Bond broken
                energy -= GAMMA(hopp, j, xp, yp, xj, yj);
                FORD(dd) {
                    int xx, yy;
                    disp(xj, yj, dd, xx, yy);
                    const int ii = lattice(xx, yy);
                    if (IS_VOID(ii))
                        rtree.update_rate(EID(ii, INVDIR(dd)), cal_rate(j, dd));
                }
            } else {
                // Events invalidated
                rtree.update_rate(EID(hopv, d), 0.0);
                rtree.update_rate(EID(j, INVDIR(d)), 0.0);
                FORD(dd) {
                    int xx, yy;
                    disp(xj, yj, dd, xx, yy);
                    const int ii = lattice(xx, yy);
                    if (IS_PART(ii))
                        rtree.update_rate(EID(j, dd), cal_rate(ii, INVDIR(dd)));
                }
            }
        }
        if (d != dv) {
            disp(xv, yv, d, xj, yj);
            j = lattice(xj, yj);
            if (IS_PART(j)) {
                // Bond created
                energy += GAMMA(hopp, j, xv, yv, xj, yj);
                FORD(dd) {
                    int xx, yy;
                    disp(xj, yj, dd, xx, yy);
                    const int ii = lattice(xx, yy);
                    if (IS_VOID(ii))
                        rtree.update_rate(EID(ii, INVDIR(dd)), cal_rate(j, dd));
                }
            } else {
                FORD(dd) {
                    int xx, yy;
                    disp(xj, yj, dd, xx, yy);
                    const int ii = lattice(xx, yy);
                    if (IS_PART(ii))
                        rtree.update_rate(EID(j, dd), cal_rate(ii, INVDIR(dd)));
                }
            }
        }
    }
    duration = 1 / rtree.get_total_rate();
}

int invarr[] = { LEFT, RIGHT, DOWN, UP };

inline void Glass2D::disp(int x, int y, int d, int &xx, int &yy) const {
    displacew(x, y, d, L, xx, yy);
}

inline int &Glass2D::lattice(int x, int y) const { return _lattice[R(x, y)]; }

#if !IS_BATCH
std::vector<int> Glass2D::get_state() const {
    std::vector<int> state(N);
    FORN(i)
        state[i] = R(X[i], Y[i]);
    return state;
}
#endif

void Glass2D::get_bonds(double* bondsx, double* bondsy) {
    int k = 0;
    FORL(xi) {
        FORL(yi) {
            const int i = lattice(xi, yi);
            if (IS_PART(i)) {
                int xj, yj;
                int j;
                // RIGHT
                disp(xi, yi, RIGHT, xj, yj);
                j = lattice(xj, yj);
                if (IS_PART(j))
                    bondsx[k] = GAMMA(i, j, xi, yi, xj, yj);
                // UP
                disp(xi, yi, UP, xj, yj);
                j = lattice(xj, yj);
                if (IS_PART(j))
                    bondsy[k] = GAMMA(i, j, xi, yi, xj, yj);
            }
            ++k;
        }
    }
}
