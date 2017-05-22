#include "RateTree.h"

RateTree::RateTree(int N_EVENTS, Rand &rand): rand(rand), N_EVENTS(N_EVENTS) {
    // h is CBT height
    // E.g. for N_EVENTS = 7, h = 4, array size is 2^(4-1)+7=15
    int n = N_EVENTS - 1, h = 1; 
    while (n) {
        ++h;
        n >>= 1;
    }
    offset = 1 << (h - 1);
    rate_arr = new double[offset + N_EVENTS]();
}

RateTree::~RateTree() {
    delete[] rate_arr;
}

//void RateTree::update_rate(int eid, double new_rate) {
//    int i = offset + eid; // Index in rate_arr
//    const double dr = new_rate - rate_arr[i];
//    if (!dr) return;
//    do { rate_arr[i] += dr; }
//        while (i >>= 1);
//}

void RateTree::update_rate(int eid, double new_rate) {
    int i = offset + eid; // Index in rate_arr
    if (rate_arr[i] == new_rate) return;
    rate_arr[i] = new_rate;
    // Iterate upwards until reaching root
    while (i >>= 1)
        rate_arr[i] = rate_arr[i << 1] + rate_arr[(i << 1) + 1];
}

int RateTree::select_event() const {
    double r = rand.ran2() * rate_arr[1];
    int i = 1;
    do {
        if (r > rate_arr[i <<= 1])  {
            // Go to right child
            // This also means the right child exists with nonzero rate
            r -= rate_arr[i++];
        }
    } while (i < offset);
    return i - offset;
}
