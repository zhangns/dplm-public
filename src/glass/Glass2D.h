#ifndef _GLASS2D_H
#define _GLASS2D_H
#include <vector>
#include "Rand.h"
#include "RateTree.h"
#include "Config.h"

#define N_DIRS  4 // Directions in 2D
#define INVDIR(d) (invarr[d])
extern int invarr[N_DIRS];
enum Direction { RIGHT, LEFT, UP, DOWN };   // 2D
typedef long long mcs_t;

inline void displace(int x, int y, int d, int &xx, int &yy) {
    if (d == RIGHT) {
        xx = x + 1;
        yy = y;
    } else if (d == LEFT) {
        xx = x - 1;
        yy = y;
    } else if (d == UP) {
        xx = x;
        yy = y + 1;
    } else {
        xx = x;
        yy = y - 1;
    }
}

inline void displacew(int x, int y, int d, int L, int &xx, int &yy) {
    if (d == RIGHT) {
        xx = x == L - 1 ? 0 : x + 1;
        yy = y;
    } else if (d == LEFT) {
        xx = x == 0 ? L - 1 : x - 1;
        yy = y;
    } else if (d == UP) {
        xx = x;
        yy = y == L - 1 ? 0 : y + 1;
    } else {
        xx = x;
        yy = y == 0 ? L - 1 : y - 1;
    }
}

// A Glass2D instance is a state machine.
class Glass2D {
public:
    // After setting global seed, construct with system parameters & initial coordinates
    // Xr, Yr, size N arrays, is the configuration to resume from
    // X0, Y0, size N arrays, is the configuration at genesis (boltzmannization)
    Glass2D(const Config &c, Rand &rand, const int *X0, const int *Y0, const int *Xr, const int *Yr);
    ~Glass2D();

    const int L, V, N, NV;

    int *const X, *const Y;                                 // Size V arrays storing coordinates of particles and voids

    void update_T(double T);                                // Update temperature (rates & duration also updated)
    void step();                                            // Evolve to next MCS

    double get_T() const;                                   // Current temperature
    int get_hopp() const;                                   // Last hopping particle index
    int get_hopv() const;                                   // Last hopping point index
    int get_hopd() const;                                   // Last hopping particle direction
    double get_time() const;                                // Time elapsed (excluding current MCS)
    mcs_t get_mcs() const;                                  // Number of MCS (excluding current MCS)
    double get_energy() const;                              // Total energy
    double get_duration() const;                            // Duration of current MCS
#if !IS_BATCH
    std::vector<int> get_state() const;                     // Unique ID of current state. for Boltzmann test
#endif
    void get_bonds(double* bondsx, double* bondsy);

    void boltzmannize();    // A priori bond energy distribution
    void reset_time();

private:

    Rand &rand;
    const Config &c;

    double T = -99.0, energy = 0.0;
    mcs_t mcs = 0;
    double duration = -99.0, time = 0.0;
    int hopp = 0, hopv = 0, dp = 0; // Current event

    // Flat array of size V storing particle/void id.
    // Particles: 0 <= i < N
    // Voids:     N <= i < V
    int *_lattice;
    int &lattice(int x, int y) const;   // 2D view of _lattice

    double *const bondTable;  // Flat array of size NxN
    int    *const bondPerms;  // Flat array of size VxN

    // Calculate the rate for particle i in the current state to hop in direction d
    double cal_rate(int i, int d) const;

    // Event id for void v hopping to direction dv is defined as N_DIRS*(v-N)+dv
    // Thus there are always N_DIRS*(V-N) distinct events.
    RateTree rtree;

    void disp(int x, int y, int d, int &xx, int &yy) const;

    void _update_energy(); // Calculate energy again
    void _boltzmannize();
    void _loadr(const int *newX, const int *newY);
    void _randr();
};


inline double  Glass2D::get_T()            const { return T; }
inline int     Glass2D::get_hopp()         const { return hopp; }
inline int     Glass2D::get_hopv()         const { return hopv; }
inline int     Glass2D::get_hopd()         const { return dp; }
inline double  Glass2D::get_time()         const { return time; }
inline mcs_t   Glass2D::get_mcs()          const { return mcs; }
inline double  Glass2D::get_energy()       const { return energy; }
inline double  Glass2D::get_duration()     const { return duration; }

#endif
