#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "MacroMode.h"
#include "fileio.h"
#include "utils.h"
#if !IS_BATCH
    #include "Monitor.h"
    #define BOLTZ_RELAX    10
    #define FILE_BOLTZ     "boltz"   // Boltzmann distribution test output
#endif

using namespace std;

MacroMode::MacroMode(ostream &out, const Config &c,
                     const char dtf[],
                     const char r0f[],
                     const char rf[],
                     const char trajf[],
                     const char ef[],
                     const char bondsf[]):
        Mode(out, c),
        fout_traj(trajf, ofstream::binary | ofstream::app),
        fout_e(ef, ofstream::binary | ofstream::app),
        fout_bonds(bondsf, ofstream::binary | ofstream::app)
#if !IS_BATCH
        ,pBoltz(c.boltz >= 0 ? new BoltzTest() : nullptr)
#endif 
    {

    strcpy(dtfile, dtf);
    strcpy(r0file, r0f);
    strcpy(rfile, rf);

    if (c.dmcs > 0) { // Use dmcs
        if (fileio::file_exists(dtfile)) // Use existing
            fileio::load_var(dtfile, dt);
    } else { // Use dt
        dt = c.dt;
        fileio::save_var(dtfile, dt);
    }

    int *X0 = nullptr, *Y0 = nullptr,
        *Xr = nullptr, *Yr = nullptr;

    if (c.resume) {
        // Xr Yr
        if (fileio::file_exists(rfile)) {
            Xr = new int[c.N]; Yr = new int[c.N];
            out << "NOTICE: loading \"" << rfile << '\"' << ENDL;
            fileio::load_r(rfile, c.N, Xr, Yr);
        } else
            out << "NOTICE: Nothing to resume from" << ENDL;
    }

    if (Xr && c.apriori) {
        // X0 Y0
        if (fileio::file_exists(r0file)) {
            X0 = new int[c.N]; Y0 = new int[c.N];
            out << "NOTICE: loading \"" << r0file << '\"' << ENDL;
            fileio::load_r(r0file, c.N, X0, Y0);
        } else {
            out << "ERROR: Cannot load \"" << r0file << "\". Exiting" << ENDL;
            exit(EXIT_FAILURE);
        }
    }

    g = new Glass2D(c, rand, X0, Y0, Xr, Yr);
    if (!Xr) {
        if (c.apriori == AP_PREHEAT)
            preheat = true;
        else // Genesis
            MacroMode::postPreheat();
    }
    if (X0) delete[] X0;
    if (Y0) delete[] Y0;
    if (Xr) delete[] Xr;
    if (Yr) delete[] Yr;
#if !IS_BATCH
    monitor = new Monitor(*g, c);
#endif
}


void MacroMode::postPreheat() {
    // Genesis
    fileio::save_r(r0file, g->N, g->X, g->Y);
}

bool MacroMode::notify() {
    if (!preheat) {
#if !IS_BATCH
        // Boltzmann test
        if (c.boltz >= 0 && dtcnt >= BOLTZ_RELAX)
            pBoltz->add_weight(g->get_state(), g->get_energy(), g->get_duration());
#endif

        if (dt > 0) { // Already known
            // Snapshot every dt
            while (g->get_time() + g->get_duration() > dtcnt * dt) {
                out << '[' << dtcnt << "]\t"
                    << "MCS " << g->get_mcs() << '\t'
                    << "t = " << dtcnt * dt << ENDL;
                fout_e << g->get_energy() / g->N << ENDL;
                if (c.savetraj)
                    fileio::put_r(fout_traj, g->N, g->X, g->Y);
                fileio::save_r(rfile, g->N, g->X, g->Y);
                if (c.savebonds) {
                    const double DUMMY = -1e200;
                    double *bondsx = new double[g->V],
                           *bondsy = new double[g->V];
                    FOR(k, g->V) {
                        bondsx[k] = DUMMY;
                        bondsy[k] = DUMMY;
                    }
                    g->get_bonds(bondsx, bondsy);
                    fileio::put_r(fout_bonds, g->V, bondsx, bondsy);
                    delete[] bondsx;
                    delete[] bondsy;
                }
                // Termination
                if (dtcnt == c.ndt)
                    return false;
                ++dtcnt;
            }
        } else if (g->get_mcs() + 1 == c.dmcs) {
            fileio::save_var(dtfile, dt = math::roundx(g->get_time()));
        }
    }
#if IS_BATCH
    return true;
#else
    return monitor->notify();
#endif
}

void MacroMode::postRun() {
    delete g; g = nullptr;
#if !IS_BATCH
    if (pBoltz) { // Dump boltz data
        pBoltz->save(FILE_BOLTZ);
        delete pBoltz;
        pBoltz = nullptr;
    }
    if (monitor) {
        monitor->end();
        delete monitor;
        monitor = nullptr;
    }
#endif
}
