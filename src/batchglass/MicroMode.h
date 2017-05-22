#ifndef _MICRO_MODE_H
#define _MICRO_MODE_H
#include <cstdint>
#include <iostream>
#include "Mode.h"

class MicroMode: public Mode {

public:
    typedef uint8_t hopd_t;
    explicit MicroMode(std::ostream &out, const Config &c,
                       const char dtfile[],
                       const char msdfile[],
                       const char sisffile[],
                       const char fpcffile[],
                       const char pretfile[]);
    ~MicroMode();
    
    void postRun() override;

private:
    char dtfile[BUF], msdfile[BUF], sisffile[BUF], fpcffile[BUF], pretfile[BUF];
    bool notify() override;
    void postPreheat() override;

    int *X0, *Y0;       // Initial configuration
    int *plist;         // nmcs - 1
    hopd_t *dlist;      // nmcs - 1
    double *tlist;      // nmcs + 1

    /*
    E.g. nmcs = 5,

    tlist[         0           1       2            3           4       5
    Time           O-----------+-------+------------+-----------+------->tTotal
    State              R0      |  R1   |    R2      |    R3     |  R4   |
    plist[                     0       1            2           3 
    dlist[

    */
};

#endif
