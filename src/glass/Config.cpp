#include <cstdlib>
#include <iostream>
#include <sstream>
#include "Config.h"
#include "utils.h"
using namespace std;

Config::Config(int argc, char *argv[]): argc(argc), argv(argv) {
    FOR(i, argc / 2)
        init(argv[2 * i + 1], argv[2 * i + 2]);
}

static int calN(int L, double phi) {
    return int(phi * L * L);
}

string Config::serialize() const {
    ostringstream ss;
    FOR2(i, 1, argc)
        ss << argv[i] << ' ';
    return ss.str();
}

void Config::init(char *k, char *v) {
    if (AEQ(k, "L")) {
        L = ATOI(v);
        N = calN(L, 0.99);
    }
    else if (AEQ(k, "N"))
        N = ATOI(v);
    else if (AEQ(k, "phi"))
        N = calN(L, ATOD(v));
    else if (AEQ(k, "phiv"))
        N = calN(L, 1.0 - ATOD(v));
#if !IS_BATCH
    else if (AEQ(k, "boltz"))
        boltz = ATOI(v);
#endif
    else if (AEQ(k, "seed"))
        seed = ATOI(v);
    else if (AEQ(k, "resume"))
        resume = ATOI(v);
    else if (AEQ(k, "apriori") || AEQ(k, "ap"))
        apriori = ATOI(v);
    else if (AEQ(k, "outmode") || AEQ(k, "out"))
        outmode = ATOI(v);
    else if (AEQ(k, "ndt"))
        ndt = ATOI(v);
    else if (AEQ(k, "dmcs"))
        dmcs = ATOI(v);
    else if (AEQ(k, "nmcs"))
        nmcs = ATOI(v);
    else if (AEQ(k, "calmsd"))
        calmsd = ATOI(v);
    else if (AEQ(k, "calsisf"))
        calsisf = ATOI(v);
    else if (AEQ(k, "calfpcf"))
        calfpcf = ATOI(v);
    else if (AEQ(k, "calpret"))
        calpret = ATOI(v);
    else if (AEQ(k, "savetraj"))
        savetraj = ATOI(v);
    else if (AEQ(k, "savebonds"))
        savebonds = ATOI(v);

    else if (AEQ(k, "gamma0") || AEQ(k, "g0") || AEQ(k, "Vmin"))
        g0 = ATOD(v);
    //else if (AEQ(k, "dgamma") || AEQ(k, "dg"))
    //    dg = ATOD(v);
    else if (AEQ(k, "T"))
        T = ATOD(v);
    else if (AEQ(k, "dt"))
        dt = ATOD(v);
    else if (AEQ(k, "frinc"))
        frinc = ATOD(v);
    else if (AEQ(k, "k")) {
        klist.clear();
        stringstream ss;
        ss << v;
        int kx;
        while (ss >> kx) {
            klist.push_back(kx);
            if (ss.peek() == ',')
                ss.ignore();
        }
    }
    else if (AEQ(k, "q")) {
        qlist.clear();
        stringstream ss;
        ss << v;
        int qx;
        while (ss >> qx) {
            qlist.push_back(qx);
            if (ss.peek() == ',')
                ss.ignore();
        }
    } else {
        cout << "ERROR: Unrecognized key \"" << k << '\"' << endl;
        exit(EXIT_FAILURE);
    }
}
