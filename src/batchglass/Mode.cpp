#include <ctime>
#include "Mode.h"
#define PREHEAT 500       // MCS per particle in preheat

using namespace std;

Mode::~Mode() { }
Mode::Mode(ostream &out, const Config &c):
    out(out), c(c), rand(c.seed ? c.seed : int(time(nullptr))) {}

void Mode::run() {
    for (;;) {
        if (preheat) {
            if (g->get_mcs() == PREHEAT * g->N) { // Terminate preheat
                rand.reset(); // Important!
                g->boltzmannize(); // Genesis
                postPreheat();
                out << "Preheat finished!" << ENDL;
                g->reset_time();
                preheat = false;
            }
        }
        // MCS
        if (!notify()) return;
        g->step();
    }
}
