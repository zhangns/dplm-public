#ifndef _MODE_H
#define _MODE_H
#include <iostream>
#include "Glass2D.h"
#include "Config.h"

class Mode {
public:
    explicit Mode(std::ostream &out, const Config &c);
    virtual ~Mode();

    // Run the simulation
    void run();

    // Called on termination
    virtual void postRun() = 0;

protected:
    std::ostream &out;
    const Config &c;
    Rand rand;
    // To be set in derived ctor
    Glass2D *g = nullptr;
    bool preheat = false;

private:
    // Called every MCS. Returns whether simulation should continue.
    virtual bool notify() = 0;

    // Called when preheat is finished
    virtual void postPreheat() = 0;

};

#endif
