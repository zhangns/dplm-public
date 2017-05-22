// A RateTree uses a complete binary tree (CBT) to maintain the rates of
// mutually exclusive probabilistic events.

#ifndef _RATETREE_H
#define _RATETREE_H
#include "Rand.h"

class RateTree {
public:
    // Construct an all-zero RateTree of N_EVENTS events.
    // Each event has a unique eid. 0 <= eid < N_EVENTS.
    RateTree(int N_EVENTS, Rand &rand);
    ~RateTree();

    // Replace the rate of a single event, eid, with new_rate, and update the CBT accordingly.
    void update_rate(int eid, double new_rate);

    // Return the sum of rates of all events.
    double get_total_rate() const;

    // Sample an event based on the current distribution of event rates.
    int select_event() const;

private:
    Rand &rand;
    const int N_EVENTS; // number of distinct events
    double *rate_arr;   // array representing the CBT
    int offset;

    /* E.g. N_EVENTS = 7, CBT height = 4
        0 (unused)
                            1                 - Total rate
                      2           3
                   4     5     6     7
                  8 9  10 11 12 13 14         - Rates of event 0 through 6

        offset = 8
        eid corresponds to rate_arr[offset + eid]

        Index relations:
                           p
                       2p     2p+1

                          c>>1
                       c0       c1
    */
};


inline double RateTree::get_total_rate() const {
    return rate_arr[1];
}


#endif
