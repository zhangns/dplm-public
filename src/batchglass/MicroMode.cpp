#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <fstream>
#include <vector>
#include <algorithm>
#include "MicroMode.h"
#include "utils.h"
#include "Timer.h"
#define THRES 0.5 // Longest observation time

using namespace std;

MicroMode::~MicroMode() {
    delete[] X0;
    delete[] Y0;
    delete[] plist;
    delete[] dlist;
    delete[] tlist;
}

MicroMode::MicroMode(ostream &out, const Config &c, const char dtf[],
                                                    const char msdf[],
                                                    const char sisff[],
                                                    const char fpcff[],
                                                    const char pretf[]):
        Mode(out, c),
        X0(new int[c.N]), Y0(new int[c.N]),
        plist(new int[c.nmcs - 1]),
        dlist(new hopd_t[c.nmcs - 1]),
        tlist(new double[c.nmcs + 1]) {

    strcpy(dtfile, dtf);
    strcpy(msdfile, msdf);
    strcpy(sisffile, sisff);
    strcpy(fpcffile, fpcff);
    strcpy(pretfile, pretf);
    tlist[0] = 0.0;
    g = new Glass2D(c, rand, nullptr, nullptr, nullptr, nullptr);
    if (c.apriori == AP_PREHEAT)
        preheat = true;
    else // Genesis
        MicroMode::postPreheat();
}

void MicroMode::postPreheat() {
    // Genesis
    FOR(i, g->N) {
        X0[i] = g->X[i];
        Y0[i] = g->Y[i];
    }
}

bool MicroMode::notify() {
    if (!preheat) {
        const mcs_t f = g->get_mcs();
        tlist[f] = g->get_time();
        if (f > 0) {
            plist[f - 1] = g->get_hopp();
            dlist[f - 1] = hopd_t(g->get_hopd());
        }
        // Termination
        if (f + 1 == c.nmcs) {
            tlist[c.nmcs] = g->get_time() + g->get_duration();
            return false;
        }
    }
    return true;
}

void MicroMode::postRun() {
    using namespace math;
    delete g; g = nullptr;
    out << "Micromode calculation..." << ENDL;
    Timer timer; timer.tic();

    // Choose dt
    const double tTotal = tlist[c.nmcs];
    const double tThres = tTotal * THRES;
    const double dt0 = c.dt > 0 ? c.dt : roundx(tTotal / c.nmcs);
    {
        ofstream fout_dt(dtfile, ofstream::binary);
        fout_dt << dt0 << '\t' << tTotal << ENDL;
    }
    vector<double> dtlist;
    dtlist.push_back(dt0);
    for (;;) {
        const double tNext = dtlist.back() * c.frinc;
        if (tNext > tThres) break;
        dtlist.push_back(tNext);
    }
    const int ndt = int(dtlist.size());

    // States at tail and head
    int *X1  = new int[c.N], *Y1  = new int[c.N],
        *Xw1 = new int[c.N], *Yw1 = new int[c.N],   // PBC
        *X2  = new int[c.N], *Y2  = new int[c.N],
        *Xw2 = new int[c.N], *Yw2 = new int[c.N];   // PBC

    if (c.calmsd) {
        double *glist = new double[ndt]();
        double gt;

        FOR(idt, ndt) {
            const double dt = dtlist[idt];
            FOR(i, c.N) {
                X1[i] = X2[i] = X0[i];
                Y1[i] = Y2[i] = Y0[i];
            }
            double t0 = 0.0;
            int k1 = 0, k2 = 0; // Time window. Indices of tlist
            // Find initial k2
            for (;;) {
                const double t = tlist[k2 + 1];
                if (t > dt) break;
                const int i = plist[k2];
                const hopd_t d = dlist[k2];
                displace(X2[i], Y2[i], d, X2[i], Y2[i]);
                ++k2;
            }
            gt = 0.0;
            FOR(i, c.N) {
                const int dx = X2[i] - X1[i],
                          dy = Y2[i] - Y1[i];
                gt += dx * dx + dy * dy;
            }
            // Slide!
            for (;;) {
                int i; hopd_t d;
                int dx, dy;
                double tau;  // Since last hop;
                const double tau1 = tlist[k1 + 1] - t0,         // Move tail
                             tau2 = tlist[k2 + 1] - (t0 + dt);  // Move head
                const bool moveTail = tau1 < tau2;
                if (moveTail) {
                    tau = tau1;
                    t0 = tlist[k1 + 1];
                } else {
                    tau = tau2;
                    t0 = tlist[k2 + 1] - dt;
                }
                glist[idt] += gt * tau;
                if (!moveTail && k2 + 1 == c.nmcs) // Coda
                    break;
                if (moveTail) {
                    i = plist[k1]; d = dlist[k1]; ++k1;
                } else {
                    i = plist[k2]; d = dlist[k2]; ++k2;
                }
                dx = X2[i] - X1[i]; dy = Y2[i] - Y1[i];
                gt -= dx * dx + dy * dy;
                if (moveTail)
                    displace(X1[i], Y1[i], d, X1[i], Y1[i]);
                else
                    displace(X2[i], Y2[i], d, X2[i], Y2[i]);
                dx = X2[i] - X1[i]; dy = Y2[i] - Y1[i];
                gt += dx * dx + dy * dy;
            }
            const double w = 1/((tTotal - dt) * c.N);
            glist[idt] *= w;
        }
        // MSD
        ofstream fout_msd(msdfile, ofstream::binary);
        FOR(idt, ndt)
            fout_msd << dtlist[idt] << '\t' << glist[idt] << NEWLINE;
        delete[] glist;
    }


    if (c.calsisf) {
        vector<double> klist(c.klist.begin(), c.klist.end());
        const int nk = int(klist.size());
        double **Flist = new double*[nk];
        FOR(ik, nk) {
            Flist[ik] = new double[ndt]();
            klist[ik] *= 2 * PI / c.L;
        }
        double **kCosTbl = new double*[nk];
        FOR(ik, nk) {
            kCosTbl[ik] = new double[c.L];
            FOR(dx, c.L)
                kCosTbl[ik][dx] = cos(klist[ik] * dx);
        }
        double *Fs = new double[nk];

        FOR(idt, ndt) {
            const double dt = dtlist[idt];
            FOR(i, c.N) {
                Xw1[i] = Xw2[i] = X0[i];
                Yw1[i] = Yw2[i] = Y0[i];
            }
            double t0 = 0.0;
            int k1 = 0, k2 = 0; // Time window. Indices of tlist
            // Find initial k2
            for (;;) {
                const double t = tlist[k2 + 1];
                if (t > dt) break;
                const int i = plist[k2];
                const hopd_t d = dlist[k2];
                displacew(Xw2[i], Yw2[i], d, c.L, Xw2[i], Yw2[i]);
                ++k2;
            }
            FOR(ik, nk) Fs[ik] = 0.0;
            FOR(i, c.N) {
                const int dxw = abs(Xw1[i] - Xw2[i]),
                          dyw = abs(Yw1[i] - Yw2[i]);
                FOR(ik, nk)
                    Fs[ik] += (kCosTbl[ik][dxw] + kCosTbl[ik][dyw]) / 2;
            }
            // Slide!
            for (;;) {
                int i; hopd_t d;
                int dxw, dyw;
                double tau;  // Since last hop;
                const double tau1 = tlist[k1 + 1] - t0,         // Move tail
                             tau2 = tlist[k2 + 1] - (t0 + dt);  // Move head
                const bool moveTail = tau1 < tau2;
                if (moveTail) {
                    tau = tau1;
                    t0 = tlist[k1 + 1];
                } else {
                    tau = tau2;
                    t0 = tlist[k2 + 1] - dt;
                }
                FOR(ik, nk)
                    Flist[ik][idt] += Fs[ik] * tau;
                if (!moveTail && k2 + 1 == c.nmcs) // Coda
                    break;
                if (moveTail) {
                    i = plist[k1]; d = dlist[k1]; ++k1;
                } else {
                    i = plist[k2]; d = dlist[k2]; ++k2;
                }
                dxw = abs(Xw1[i] - Xw2[i]); dyw = abs(Yw1[i] - Yw2[i]);
                FOR(ik, nk)
                    Fs[ik] -= (kCosTbl[ik][dxw] + kCosTbl[ik][dyw]) / 2;
                if (moveTail) {
                    displacew(Xw1[i], Yw1[i], d, c.L, Xw1[i], Yw1[i]);
                } else {
                    displacew(Xw2[i], Yw2[i], d, c.L, Xw2[i], Yw2[i]);
                }
                dxw = abs(Xw1[i] - Xw2[i]); dyw = abs(Yw1[i] - Yw2[i]);
                FOR(ik, nk)
                    Fs[ik] += (kCosTbl[ik][dxw] + kCosTbl[ik][dyw]) / 2;
            }
            const double w = 1 / ((tTotal - dt) * c.N);
            FOR(ik, nk)
                Flist[ik][idt] *= w;
        }
        delete[] Fs;


        if (c.calfpcf) {
            vector<int> qlist0(c.qlist);
            qlist0.push_back(0); // For \chi_4
            vector<double> qlist(qlist0.begin(), qlist0.end());
            const int nq = int(qlist.size());
            double ***Slist = new double**[nq];
            FOR(iq, nq) {
                qlist[iq] *= 2 * PI / c.L;
                Slist[iq] = new double*[nk];
                FOR(ik, nk)
                    Slist[iq][ik] = new double[ndt]();
            }
            double **qCosTbl = new double*[nq];
            FOR(iq, nq) {
                qCosTbl[iq] = new double[c.L];
                FOR(dx, c.L)
                    qCosTbl[iq][dx] = cos(qlist[iq] * dx);
            }
            double **S4 = new double*[nq];
            FOR(iq, nq)
                S4[iq] = new double[nk];

            FOR(idt, ndt) {
                const double dt = dtlist[idt];
                FOR(i, c.N) {
                    Xw1[i] = Xw2[i] = X0[i];
                    Yw1[i] = Yw2[i] = Y0[i];
                }
                double t0 = 0.0;
                int k1 = 0, k2 = 0; // Time window. Indices of tlist
                // Find initial k2
                for (;;) {
                    const double t = tlist[k2 + 1];
                    if (t > dt) break;
                    const int i = plist[k2];
                    const hopd_t d = dlist[k2];
                    displacew(Xw2[i], Yw2[i], d, c.L, Xw2[i], Yw2[i]);
                    ++k2;
                }
                FOR(iq, nq)
                    FOR(ik, nk)
                        S4[iq][ik] = 0.0;
                FOR(i, c.N) {
                    const int dxw = abs(Xw1[i] - Xw2[i]),
                              dyw = abs(Yw1[i] - Yw2[i]);
                    FOR(iq, nq)
                        FOR(ik, nk)
                            S4[iq][ik] += (qCosTbl[iq][Xw1[i]] + qCosTbl[iq][Yw1[i]]) / 2 *
                                    ((kCosTbl[ik][dxw] + kCosTbl[ik][dyw]) / 2 - Flist[ik][idt]);
                }
                // Slide!
                for (;;) {
                    int i; hopd_t d;
                    int dxw, dyw;
                    double tau;  // Since last hop;
                    const double tau1 = tlist[k1 + 1] - t0,         // Move tail
                                 tau2 = tlist[k2 + 1] - (t0 + dt);  // Move head
                    const bool moveTail = tau1 < tau2;
                    if (moveTail) {
                        tau = tau1;
                        t0 = tlist[k1 + 1];
                    } else {
                        tau = tau2;
                        t0 = tlist[k2 + 1] - dt;
                    }
                    FOR(iq, nq) {
                        FOR(ik, nk) {
                            const double s4 = S4[iq][ik];
                            Slist[iq][ik][idt] += s4 * s4 * tau;
                        }
                    }
                    if (!moveTail && k2 + 1 == c.nmcs) // Coda
                        break;
                    if (moveTail) {
                        i = plist[k1]; d = dlist[k1]; ++k1;
                    } else {
                        i = plist[k2]; d = dlist[k2]; ++k2;
                    }
                    dxw = abs(Xw1[i] - Xw2[i]); dyw = abs(Yw1[i] - Yw2[i]);
                    FOR(iq, nq)
                        FOR(ik, nk)
                            S4[iq][ik] -= (qCosTbl[iq][Xw1[i]] + qCosTbl[iq][Yw1[i]]) / 2 *
                                    ((kCosTbl[ik][dxw] + kCosTbl[ik][dyw]) / 2 - Flist[ik][idt]);
                    if (moveTail)
                        displacew(Xw1[i], Yw1[i], d, c.L, Xw1[i], Yw1[i]);
                    else
                        displacew(Xw2[i], Yw2[i], d, c.L, Xw2[i], Yw2[i]);
                    dxw = abs(Xw1[i] - Xw2[i]); dyw = abs(Yw1[i] - Yw2[i]);
                    FOR(iq, nq)
                        FOR(ik, nk)
                            S4[iq][ik] += (qCosTbl[iq][Xw1[i]] + qCosTbl[iq][Yw1[i]]) / 2 *
                                    ((kCosTbl[ik][dxw] + kCosTbl[ik][dyw]) / 2 - Flist[ik][idt]);
                }
                const double w = 1 / ((tTotal - dt) * c.N);
                FOR(iq, nq)
                    FOR(ik, nk)
                    Slist[iq][ik][idt] *= w;
            }
            delete[] S4;

            FOR(iq, nq) {
                FOR(ik, nk) {
                    char Sfile[BUF];
                    snprintf(Sfile, BUF, "%s_%02d_%02d", fpcffile, c.klist[ik], qlist0[iq]);
                    ofstream fout(Sfile, ofstream::binary);
                    FOR(idt, ndt)
                        fout << dtlist[idt] << '\t' << Slist[iq][ik][idt] << NEWLINE;
                }
            }
            FOR(iq, nq) {
                FOR(ik, nk)
                    delete[] Slist[iq][ik];
                delete[] Slist[iq];
                delete[] qCosTbl[iq];
            }
            delete[] Slist;
            delete[] qCosTbl;
        }

        FOR(ik, nk) {
            char kfile[BUF];
            snprintf(kfile, BUF, "%s_%02d", sisffile, c.klist[ik]);
            ofstream fout_k(kfile, ofstream::binary);
            FOR(idt, ndt)
                fout_k << dtlist[idt] << '\t' << Flist[ik][idt] << NEWLINE;
        }
        FOR(ik, nk) {
            delete[] Flist[ik];
            delete[] kCosTbl[ik];
        }
        delete[] Flist;
        delete[] kCosTbl;
    }


    if (c.calpret) {
        int nReturn = 0, nNonReturn = 0;
        hopd_t *lastd = new hopd_t[c.N];
        const hopd_t UNDEF = 0xff;
        FOR(i, c.N)
            lastd[i] = UNDEF;
        const int nHop = c.nmcs - 1;
        FOR(k, nHop) {
            const int i = plist[k];
            const hopd_t d = dlist[k];
            if (lastd[i] != UNDEF) {
                if (d == INVDIR(lastd[i]))
                    ++nReturn;
                else
                    ++nNonReturn;
            }
            lastd[i] = d;
        }
        ofstream fout_pret(pretfile, ofstream::binary);
        fout_pret << nReturn << '\t' << nNonReturn << '\t' << nHop << ENDL;
    }

    delete[] X1;  delete[] Y1;
    delete[] Xw1; delete[] Yw1;
    delete[] X2;  delete[] Y2;
    delete[] Xw2; delete[] Yw2;

    time_t tEnd = timer.toc();
    out << "Micromode complete at " << ctime(&tEnd)
        << "Time elapsed: " << timer.nsec() << 's' << ENDL;
}
