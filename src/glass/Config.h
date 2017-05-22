#ifndef _CONFIG_H
#define _CONFIG_H
#include "utils.h"
#include <vector>
#include <string>

enum APRIORI_MODE { AP_OFF, AP_INSTANT, AP_PREHEAT };
enum OUTPUT_MODE  { OUT_MACRO, OUT_MICRO };

class Config {
public:
    explicit Config(int argc, char *argv[]);

    int argc;
    char **argv;

    std::string serialize() const;

    int
        L         = 100,                          // System size
        N         = 9900,                         // No. of particles
#if !IS_BATCH                                        
        boltz     = -1,                           // No. of frozen particles (for Boltzmann test in macromode. -1 = turn off test)
#endif                                               
        seed      = 42,                           // Random seed
        resume    = 0,                            // Whether try to resume simulation (for macromode)
        apriori   = AP_INSTANT,                   // A priori bond energies (0 = turn off; 1 = instant; 2 = instant + preheat)
        outmode   = OUT_MACRO,                    // Output mode
        ndt       = 10,                           // No. of dt to simulate. (for macromode. 0 = infinite)
        dmcs      = 0,                            // (for macromode, overrides dt. 0 = dummy)
        nmcs      = 10000000,                     // (for micromode)...
        calmsd    = 0,
        calsisf   = 0,
        calfpcf   = 0,
        calpret   = 0,
        savetraj  = 1,
        savebonds = 0;
                                                     
    double
        g0      = -0.5,                         // Energy offset
        dg      = 1.0,                          // Energy range
        T       = 0.20,                         // Temperature
        dt      = 1e6,                          // Time interval. (if micromode, -1 = auto)
        frinc   = 1.4;                          // Fractional increment

    std::vector<int> klist = {25};              // For F_s(k,t)
    std::vector<int> qlist = {};                // For S_4(k,q,t). q = 0 is always used, omitted

private:
    // Set relevant member variables from a KV pair
    void init(char *k, char *v);
};

#endif
