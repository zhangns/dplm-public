#ifndef _MACRO_MODE_H
#define _MACRO_MODE_H
#include <iostream>
#include <fstream>
#include "Mode.h"
#if !IS_BATCH
    #include "BoltzTest.h"
    #include "Monitor.h"
#endif

class MacroMode : public Mode {
public:
    explicit MacroMode(std::ostream &out, const Config &c,
                       const char dtfile[],
                       const char r0file[],
                       const char rfile[],
                       const char trajfile[],
                       const char efile[],
                       const char bondsfile[]);

    void postRun() override;

private:
    char dtfile[BUF], r0file[BUF], rfile[BUF];
    std::ofstream fout_traj, fout_e, fout_bonds;
#if !IS_BATCH
    BoltzTest *pBoltz;
    Monitor *monitor = nullptr;
#endif

    bool notify() override;
    void postPreheat() override;

    int dtcnt = 0;
    double dt = -1.0; // Dummy
};

#endif
