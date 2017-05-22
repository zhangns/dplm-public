// Entry point of the simulation program.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <vector>
#include <thread>
#include <string>
#include "Config.h"
#include "Timer.h"
#include "Mode.h"
#include "fileio.h"
#include "MacroMode.h"
#include "MicroMode.h"
#include "Semaphore.h"
#define FILE_BATCH      "batch.txt"
#define FILE_STDOUT     "stdout"
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
mutex mtxStdout;
Semaphore *semaphore = nullptr;

// One worker thread for each run
static void worker(string tagstr, const Config &c) {
    const char *const tag = tagstr.c_str();
    {
        lock_guard<mutex> lock(mtxStdout);
        cout << "Starting " << tag << ": " << c.serialize() << endl;
    }
    fileio::mkdirx(tag);
    char _FILE_DT[BUF];
    snprintf(_FILE_DT, BUF, "%s/%s", tag, FILE_DT);
    char _FILE_R0[BUF];
    snprintf(_FILE_R0, BUF, "%s/%s", tag, FILE_R0);
    char _FILE_R[BUF];
    snprintf(_FILE_R, BUF, "%s/%s", tag, FILE_R);
    char _FILE_TRAJ[BUF];
    snprintf(_FILE_TRAJ, BUF, "%s/%s", tag, FILE_TRAJ);
    char _FILE_E[BUF];
    snprintf(_FILE_E, BUF, "%s/%s", tag, FILE_E);
    char _FILE_BONDS[BUF];
    snprintf(_FILE_BONDS, BUF, "%s/%s", tag, FILE_BONDS);
    char _FILE_MSD[BUF];
    snprintf(_FILE_MSD, BUF, "%s/%s", tag, FILE_MSD);
    char _FILE_SISF[BUF];
    snprintf(_FILE_SISF, BUF, "%s/%s", tag, FILE_SISF);
    char _FILE_FPCF[BUF];
    snprintf(_FILE_FPCF, BUF, "%s/%s", tag, FILE_FPCF);
    char _FILE_PRET[BUF];
    snprintf(_FILE_PRET, BUF, "%s/%s", tag, FILE_PRET);
    char _FILE_STDOUT[BUF];
    snprintf(_FILE_STDOUT, BUF, "%s/%s", tag, FILE_STDOUT);
    ofstream out(_FILE_STDOUT, ofstream::app | ofstream::binary);

    Timer timer;
    time_t tStart = timer.tic();
    out << c.serialize() << NEWLINE
        << "Simulation started at " << ctime(&tStart) << ENDL;

    Mode *mode =
        c.outmode == OUT_MACRO ?
        static_cast<Mode*>(new MacroMode(out, c, _FILE_DT,
                                                 _FILE_R0,
                                                 _FILE_R,
                                                 _FILE_TRAJ,
                                                 _FILE_E,
                                                 _FILE_BONDS)):
        static_cast<Mode*>(new MicroMode(out, c, _FILE_DT,
                                                 _FILE_MSD,
                                                 _FILE_SISF,
                                                 _FILE_FPCF,
                                                 _FILE_PRET));
    mode->run();

    time_t tEnd = timer.toc();
    out << "Simulation complete at " << ctime(&tEnd)
        << "Time elapsed: " << timer.nsec() << 's' << ENDL;

    mode->postRun();
    delete mode;
    {
        lock_guard<mutex> lock(mtxStdout);
        cout << tag << " complete" << endl;
    }
    if (semaphore) semaphore->release();
}

int main(int argc, char *argv[]) {
    // Load configurations
    const char filebatch[] = FILE_BATCH;
    const char *batchfile = argc >= 2 ? argv[1] : filebatch;
    if (!fileio::file_exists(batchfile)) {
        cout << "ERROR: " << batchfile << " not found! Exiting" << endl;
        return EXIT_FAILURE;
    }
    vector<Config*> configs;
    vector<string> tags;
    ifstream batchf(batchfile);
    int cargc;
    string line;
    while (getline(batchf, line)) {
        istringstream ss(line);
        string tag;
        ss >> tag;
        if (tag.empty() || tag[0] == '#')
            continue;
        tags.push_back(tag);
        cargc = 1;
        char **cargv = new char*[BUF];
        FOR(i, BUF)
            cargv[i] = new char[BUF];
        while (ss >> cargv[cargc])
            ++cargc;
        configs.push_back(new Config(cargc, cargv));
    }
    batchf.close();
    if (configs.empty())
        return EXIT_FAILURE;

    if (argc >= 3) {
        const int nThreads = ATOI(argv[2]);
        cout << "Using " << nThreads << " threads" << endl;
        semaphore = new Semaphore(nThreads);
    }

    int tDelay = 0;
    if (argc >= 4) {
        tDelay = ATOI(argv[3]);
        cout << "With delay " << tDelay << 's' << endl;
    }

    Timer timer;
    time_t tStart = timer.tic();
    cout << "Batch started at " << ctime(&tStart) << endl;

    vector<thread*> threads;
    const int nRuns = int(configs.size());
    FOR(i, nRuns) {
        if (semaphore) semaphore->acquire();
        threads.push_back(new thread(&worker, tags[i], *configs[i]));
        if (tDelay > 0)
            this_thread::sleep_for(chrono::seconds(tDelay));
    }
    FOR(i, nRuns)
        threads[i]->join();

    time_t tEnd = timer.toc();
    cout << "Batch complete at " << ctime(&tEnd)
         << "Time elapsed: " << timer.nsec() << 's' << endl;
    return EXIT_SUCCESS;
}
