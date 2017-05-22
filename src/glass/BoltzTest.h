#ifndef _BOLTZTEST_H
#define _BOLTZTEST_H
#include <map>
#include <vector>

// Boltzmann distribution test
class BoltzTest {
    typedef std::vector<int> State; // position of each particle
    typedef std::pair<double, double> EWPair; // energy-weight pair
    // Map from State to EWPair
    typedef std::map<State, EWPair> EnsembleMap;
    EnsembleMap ensemble;

public:
    // Add weight to a specific state
    void add_weight(State state, double energy, double weight);

    // Dump sorted energy-weight pairs in a text file
    void save(const char filename[]);
};

#endif
