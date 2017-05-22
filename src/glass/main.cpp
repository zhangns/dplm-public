// Entry point of the simulation program.

#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Config.h"
#include "Timer.h"
#include "Mode.h"
#include "MacroMode.h"
#include "MicroMode.h"
// Macromode
#define FILE_DT         "dt"      // Snapshot interval
#define FILE_R0         "r0"      // Initialize configuration
#define FILE_R          "r"       // Current configuration
#define FILE_TRAJ       "traj"    // Trajectory
#define FILE_E          "e"       // Energy per particle
#define FILE_BONDS      "bonds"   // Bond energies
// Micromode
#define FILE_MSD        "msd"     // Mean square displacement
#define FILE_SISF       "sisf"    // Self-intermediate scattering function
#define FILE_FPCF       "fpcf"    // Four-point correlation function
#define FILE_PRET       "pret" 

using namespace std;

int main(int argc, char *argv[]) {
    ostream &out = cout;
    const Config c(argc, argv);

    Timer timer;
    time_t tStart = timer.tic();
    out << c.serialize() << NEWLINE
        << "Simulation started at " << ctime(&tStart) << ENDL;

    Mode *mode =
        c.outmode == OUT_MACRO ?
        static_cast<Mode*>(new MacroMode(out, c, FILE_DT,
                                                 FILE_R0,
                                                 FILE_R,
                                                 FILE_TRAJ,
                                                 FILE_E,
                                                 FILE_BONDS)):
        static_cast<Mode*>(new MicroMode(out, c, FILE_DT,
                                                 FILE_MSD,
                                                 FILE_SISF,
                                                 FILE_FPCF,
                                                 FILE_PRET));
    mode->run();
    
    time_t tEnd = timer.toc();
    out << "Simulation complete at " << ctime(&tEnd)
        << "Time elapsed: " << timer.nsec() << 's' << ENDL;
    
    mode->postRun();
    delete mode;
    return EXIT_SUCCESS;
}
