#include <utility>
#include <fstream>
#include <algorithm>
#include "BoltzTest.h"
#include "utils.h"
using namespace std;

void BoltzTest::add_weight(State state, double energy, double weight) {
    EWPair &ew = ensemble[state];
    if (!ew.second) // if this is a state unseen before
        ew.first = energy;
    ew.second += weight;
    //PRINTLN(ensemble.size());
    //if (abs(energy - ew.first) > 1e-4)
    //    cout << "Error " << energy << ' ' << ew.first << endl;
}

void BoltzTest::save(const char filename[]) {
    size_t n = ensemble.size();
    EWPair *pairs = new EWPair[n];
    int i = 0;
    for (EnsembleMap::iterator it = ensemble.begin(); it != ensemble.end(); ++it, ++i)
        pairs[i] = it->second;
    sort(pairs, pairs + n);
    ofstream fout(filename, ofstream::binary);
    for (size_t j = 0; j < n; ++j)
        fout << pairs[j].first << ' ' << pairs[j].second << NEWLINE;
    delete[] pairs;
}
