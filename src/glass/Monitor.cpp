#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <png.h>
#if _WIN32
    #include <Windows.h>
#endif
#include <FL/gl.h>
#include <FL/Fl.H>
#include "Monitor.h"
#include "utils.h"
#include "Glass2D.h"

Monitor *Monitor::psMonitor = nullptr;
Monitor *Monitor::getMonitor() { return psMonitor; }

void flWorker()                         { Monitor::getMonitor()->_run(); }
void radDisplay_cb(Fl_Widget *, long v) { Monitor::getMonitor()->setDispMode(int(v)); }
void radShape_cb(Fl_Widget *, long v)   { Monitor::getMonitor()->setShape(int(v)); }
void radPcolor_cb(Fl_Widget *, long v)  { Monitor::getMonitor()->setPcolor(int(v)); }
void radVcolor_cb(Fl_Widget *, long v)  { Monitor::getMonitor()->setVcolor(int(v)); }
void spinT_cb(Fl_Widget *)              { Monitor::getMonitor()->onSetT(); }
void cntVdt_cb(Fl_Widget *)             { Monitor::getMonitor()->onSetVdt(); }
void cntDelay_cb(Fl_Widget *)           { Monitor::getMonitor()->onSetDelay(); }
void cntRadius_cb(Fl_Widget *)          { Monitor::getMonitor()->onSetRadius(); }
void cntLinewidth_cb(Fl_Widget *)       { Monitor::getMonitor()->onSetLinewidth(); }
void cntDeviate_cb(Fl_Widget *)         { Monitor::getMonitor()->onSetDeviate(); }
void cntSigThres_cb(Fl_Widget *)        { Monitor::getMonitor()->onSetSigThres(); }
void cntSigTol_cb(Fl_Widget *)          { Monitor::getMonitor()->onSetSigTol(); }
void btnPause_cb(Fl_Widget *)           { Monitor::getMonitor()->onPause(); }
void btnCapture_cb(Fl_Widget *)         { Monitor::getMonitor()->onCapture(); }
void window_cb(Fl_Widget *, void *)     { Monitor::getMonitor()->callback(); }
void exit_msg_handler(void *)           { Monitor::getMonitor()->_exit(); }

// GUI
Monitor::Window::Window(int w, int h, const char title[]):
    Fl_Window(w, h, title) {}

void Monitor::Window::resize(int X, int Y, int W, int H) {
    Fl_Window::resize(X, Y, W, H);
    getMonitor()->onResize();
}

void Monitor::onResize() {
    char buf[BUF];
    snprintf(buf, BUF, "%dx%d", canvas->w(), canvas->h());
    boxSize->copy_label(buf);
}

void Monitor::_run() {
    const std::string title = c.serialize();
    window = new Window(1000, 700, title.c_str());
    canvas = new Canvas(0, 0, 700, 700);
    canvas->end();

    int x = 700, w = 90, w2 = 190, y = 5, h = 20, dh = 4;
    btnPause = new Fl_Toggle_Button(x + 5, x - 40, 90, 30, "Pause");
    btnPause->callback(&btnPause_cb);
    btnCapture = new Fl_Toggle_Button(x + 100, x - 40, 90, 30, "Capture");
    btnCapture->callback(&btnCapture_cb);

    lblMCS = new Fl_Box(x, y, w, h, "MCS");
    boxMCS = new Fl_Box(x + w, y, w2, h);
    y += h + dh;
    lblTime = new Fl_Box(x, y, w, h, "t");
    boxTime = new Fl_Box(x + w, y, w2, h);
    y += h + dh;
    lblT = new Fl_Box(x, y, w, h, "T");
    spinT = new Fl_Spinner(x + w, y, w2, h);
    spinT->value(g.get_T());
    spinT->range(0.01, 1.00);
    spinT->step(0.01);
    spinT->callback(&spinT_cb);
    y += h + dh;
    lblE = new Fl_Box(x, y, w, h, "E");
    boxE = new Fl_Box(x + w, y, w2, h);
    y += h + dh;
    lblMSD = new Fl_Box(x, y, w, h, "MSD");
    boxMSD = new Fl_Box(x + w, y, w2, h);
    y += h + dh;
    lblPret = new Fl_Box(x, y, w, h, "Pret");
    boxPret = new Fl_Box(x + w, y, w2, h);
    y += h + dh;
    lblSize = new Fl_Box(x, y, w, h, "Size");
    boxSize = new Fl_Box(x + w, y, w2, h);
    y += h + dh;
    lblDxMax = new Fl_Box(x, y, w, h, "dxmax");
    boxDxMax = new Fl_Box(x + w, y, w2, h);

    y += h + dh;
    lblVdt = new Fl_Box(x, y, w, h, "Refresh");
    cntVdt = new Fl_Simple_Counter(x + w, y, 100, h);
    cntVdt->range(0.5, 10.0);
    cntVdt->step(0.5);
    cntVdt->value(1.0);
    cntVdt->callback(&cntVdt_cb);

    cntVdtScale = new Fl_Simple_Counter(x + w + 100, y, 100, h);
    cntVdtScale->range(-16.0, 8.0);
    cntVdtScale->step(1.0);
    cntVdtScale->value(-2.0);
    cntVdtScale->callback(&cntVdt_cb);
    cntVdt->do_callback(); // Update vdt

    y += h + dh;
    lblDelay = new Fl_Box(x, y, w, h, "Delay");
    cntDelay = new Fl_Simple_Counter(x + w, y, 100, h);
    // x0.1s
    cntDelay->range(0.0, 10.0);
    cntDelay->step(1.0);
    cntDelay->value(0.0);
    cntDelay->callback(&cntDelay_cb);

    y += h + dh;
    pGrpShape = new Fl_Group(x, y, 300, h);
    lblShape = new Fl_Box(x, y, w, h, "Shape");
    radShapeSquare = new Fl_Radio_Round_Button(x + w, y, 80, h, "Square");
    radShapeSquare->callback(&radShape_cb, SP_SQUARE);
    radShapeCircle = new Fl_Radio_Round_Button(x + w + 100, y, 80, h, "Circle"); radShapeCircle->value(1);
    radShapeCircle->callback(&radShape_cb, SP_CIRCLE);
    pGrpShape->end();

    y += h + dh;
    lblRadius = new Fl_Box(x, y, w, h, "Radius");
    cntRadius = new Fl_Simple_Counter(x + w, y, 100, h);
    cntRadius->range(0.00, 0.50);
    cntRadius->step(0.01);
    cntRadius->value(radius);
    cntRadius->callback(&cntRadius_cb);

    y += h + dh;
    pGrpParticle = new Fl_Group(x, y, 300, h);
    lblShape = new Fl_Box(x, y, w, h, "Particle");
    radPPlain = new Fl_Radio_Round_Button(x + w, y, 60, h, "Plain");
    radPPlain->callback(&radPcolor_cb, PC_PLAIN);
    radPRainbow = new Fl_Radio_Round_Button(x + w + 60 * 1, y, 80, h, "Rainbow"); radPRainbow->value(1);
    radPRainbow->callback(&radPcolor_cb, PC_RAINBOW);
    radPDr = new Fl_Radio_Round_Button(x + w + 60 + 80, y, 60, h, "dr");
    radPDr->callback(&radPcolor_cb, PC_DR);
    pGrpParticle->end();

    y += h + dh;
    pGrpVoid = new Fl_Group(x, y, 300, h);
    lblShape = new Fl_Box(x, y, w, h, "Void");
    radVoidBlack = new Fl_Radio_Round_Button(x + w, y, 60, h, "Black"); radVoidBlack->value(1);
    radVoidBlack->callback(&radVcolor_cb, VC_BLACK);
    radVoidWhite = new Fl_Radio_Round_Button(x + w + 60, y, 60, h, "White");
    radVoidWhite->callback(&radVcolor_cb, VC_WHITE);
    radVoidWhiteBB = new Fl_Radio_Round_Button(x + w + 60*2, y, 80, h, "WhiteBB");
    radVoidWhiteBB->callback(&radVcolor_cb, VC_WHITEBB);
    pGrpVoid->end();

    y += h + dh;
    lblLinewidth = new Fl_Box(x, y, w, h, "Linewidth");
    cntLinewidth = new Fl_Simple_Counter(x + w, y, 100, h);
    cntLinewidth->range(1.0, 5.0);
    cntLinewidth->step(1.0);
    cntLinewidth->value(linewidth);
    cntLinewidth->callback(&cntLinewidth_cb);

    y += h + dh;
    lblDeviate = new Fl_Box(x, y, w, h, "Deviate");
    cntDeviate = new Fl_Simple_Counter(x + w, y, 100, h);
    cntDeviate->range(0.00, 0.20);
    cntDeviate->step(0.01);
    cntDeviate->value(deviate);
    cntDeviate->callback(&cntDeviate_cb);

    //y += h + dh;
    //lblSigmoid = new Fl_Box(x, y, w, h, "Sigmoid");
    //cntSigThres = new Fl_Simple_Counter(x + w, y, 100, h);
    //cntSigThres->range(0.00, 0.98);
    //cntSigThres->step(0.02);
    //cntSigThres->value(sigThres);
    //cntSigThres->callback(&cntSigThres_cb);

    //cntSigTol = new Fl_Simple_Counter(x + w + 100, y, 100, h);
    //cntSigTol->range(0.00, 1.00);
    //cntSigTol->step(0.02);
    //cntSigTol->value(sigTol);
    //cntSigTol->callback(&cntSigTol_cb);

    y += h + dh;
    lblDisplay = new Fl_Box(x, y, w, h, "Display");
    radDispNone = new Fl_Radio_Round_Button(x + w, y, w2, h, "None"); radDispNone->value(1);
    radDispNone->callback(&radDisplay_cb, DM_NONE);
    y += h + dh;
    radDispTraj = new Fl_Radio_Round_Button(x + w, y, w2, h, "Trajectory");
    radDispTraj->callback(&radDisplay_cb, DM_TRAJ);
    y += h + dh;
    radDispPDisp = new Fl_Radio_Round_Button(x + w, y, w2, h, "Particle displacement");
    radDispPDisp->callback(&radDisplay_cb, DM_PDISP);
    y += h + dh;
    radDispVDisp = new Fl_Radio_Round_Button(x + w, y, w2, h, "Void displacement");
    radDispVDisp->callback(&radDisplay_cb, DM_VDISP);

    window->end();
    window->resizable(canvas);
    window->callback(&window_cb);

    static char str[] = "glass"; // dummy
    char *p = str; char **argv = &p;
    window->show(1, argv);
    Fl::lock(); // initialize threading
    isFlReady = true;
    Fl::run();
}

void Monitor::onPause()     { isPaused = btnPause->value() != 0; }
void Monitor::onSetT()      { newT = spinT->value(); }
void Monitor::onSetDelay()  { delay = int(cntDelay->value()); }
void Monitor::_exit()       { window->hide(); }

void Monitor::onCapture() {
    if (isPaused && !isCapturing) {
        // Capture single frame
        _capture();
        btnCapture->value(0);
    } else if ((isCapturing = btnCapture->value() != 0) == false)
        btnCapture->copy_label("Capture");
}

void Monitor::onSetRadius() {
    radius = float(cntRadius->value());
    canvas->redraw();
}

void Monitor::onSetLinewidth() {
    linewidth = float(cntLinewidth->value());
    canvas->redraw();
}

void Monitor::onSetDeviate() {
    deviate = float(cntDeviate->value());
    canvas->redraw();
}

void Monitor::onSetSigThres() {
    sigThres = cntSigThres->value();
    canvas->redraw();
}

void Monitor::onSetSigTol() {
    sigTol = cntSigTol->value();
    canvas->redraw();
}

void Monitor::setDispMode(int _dispMode) {
    dispMode = _dispMode;
    canvas->redraw();
}

void Monitor::setShape(int _shape) {
    shape = _shape;
    canvas->redraw();
}

void Monitor::setPcolor(int _pcolor) {
    pcolor = _pcolor;
    canvas->redraw();
}

void Monitor::setVcolor(int _vcolor) {
    vcolor = _vcolor;
    canvas->redraw();
}

void Monitor::onSetVdt() {
    vtimer = g.get_time(); // Refresh immediately
    vdt = cntVdt->value() * pow(10.0, cntVdtScale->value());
}

void Monitor::callback() {
    if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape)
        return;
    isClosing = true;
    if (isPaused)
        isPaused = false;
}

void Monitor::end() {
    Fl::awake(&exit_msg_handler);
    flThread->join();
}

// Graphics
struct RGB {
    float r, g, b;
};

RGB dark(RGB rgb, float r) {
    return {rgb.r * r, rgb.g * r, rgb.b * r};
}

// A rainbow colormap
// v in [0, 1]
RGB rainbow(float v) {
    // Stepped interpolation
    const static float
        V[] = {0, 0.125, 0.375, 0.625, 0.875,    1},
        R[] = {1,     1,   0.3,     0,   0.3,    1},
        G[] = {0,     1,     1,     1,   0.3,  0.3},
        B[] = {0,     0,   0.3,     1,     1,    1};
    int i = 0;
    while (v > V[i + 1])
        ++i;
    const int j = i + 1;
    const float x = (v - V[i]) / (V[j] - V[i]);
    return {
        R[i] + (R[j] - R[i]) * x,
        G[i] + (G[j] - G[i]) * x,
        B[i] + (B[j] - B[i]) * x,
    };
}

Monitor::~Monitor() {
    psMonitor = nullptr;
    delete[] X_;  delete[] Y_;
    delete[] X__; delete[] Y__;
    delete[] Hopd;
}

Monitor::Monitor(Glass2D &g, const Config &c) :
    c(c), g(g), rand(int(time(nullptr))),
    X_(new int[g.V]), Y_(new int[g.V]),
    X__(new int[g.V]), Y__(new int[g.V]),
    Hopd(new int[g.N]()) {
    psMonitor = this;
    FOR(i, g.V) {
        X__[i] = X_[i] = g.X[i];
        Y__[i] = Y_[i] = g.Y[i];
    }
    flThread = new std::thread(&flWorker);
    while (!isFlReady) {}
}

bool Monitor::notify() {
    using namespace math;
    using std::this_thread::sleep_for;
    using std::chrono::milliseconds;

    if (isClosing) return false;

    if (newT > 0) {
        g.update_T(newT);
        newT = -1;
    }

    const int
        p = g.get_hopp(),
        v = g.get_hopv(),
        dp = g.get_hopd();

    ++nHop;
    if (dp == INVDIR(Hopd[p])) ++nReturn;
    Hopd[p] = dp;

    vvec.push_back(v);
    vdvec.push_back(INVDIR(dp));

    // Update monitor every vdt
    if (g.get_time() + g.get_duration() > vtimer) {
        if (delay) sleep_for(milliseconds(delay * 100));
        Fl::lock();
        double dr2sum = 0.0; // Total square displacement
        int dxmax = 0;
        FOR(i, g.N) {
            const int
                dx = period_dist(X_[i], g.X[i], g.L),
                dy = period_dist(Y_[i], g.Y[i], g.L);
            const double dr2 = square(dx) + square(dy);
            if (dx > dxmax) dxmax = dx;
            dr2sum += dr2;
        }
        FOR(i, g.V) {
            X__[i] = X_[i]; Y__[i] = Y_[i];
            X_[i] = g.X[i]; Y_[i] = g.Y[i];
        }
        // Update UI
        char buf[BUF];
        snprintf(buf, BUF, "%lld", g.get_mcs());
        boxMCS->copy_label(buf);
        snprintf(buf, BUF, "%g", vtimer);
        boxTime->copy_label(buf);
        snprintf(buf, BUF, "%g", g.get_energy() / g.N);
        boxE->copy_label(buf);
        snprintf(buf, BUF, "%g", dr2sum / g.N);
        boxMSD->copy_label(buf);
        snprintf(buf, BUF, "%g", double(nReturn) / nHop);
        boxPret->copy_label(buf);
        snprintf(buf, BUF, "%d", dxmax);
        boxDxMax->copy_label(buf);
        nReturn = nHop = 0;

        // Redraw synchronously
        hasRedrawn = false;
        if (window->visible()) {
            canvas->redraw();
            Fl::awake();
            Fl::unlock();
            while (!hasRedrawn) {}
        } else {
            // In case window is minimized
            Fl::unlock();
        }
        vvec.clear();
        vdvec.clear();

        // Increment timer
        do
            vtimer += vdt;
        while (g.get_time() + g.get_duration() > vtimer);
    } // vtimer

    while (isPaused) sleep_for(milliseconds(100));
    return true;
}

Monitor::Canvas::Canvas(int X, int Y, int W, int H) :
        Fl_Gl_Window(X, Y, W, H) {
    mode(FL_RGB | FL_SINGLE);
}

void Monitor::Canvas::draw() {
    getMonitor()->draw();
}

namespace gl {
    const static int N_SEG = 60;
    const static float w = float(2 * math::PI / N_SEG);

    void bgWhite() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void bgBlack() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void setBlack() { glColor3f(0.0f, 0.0f, 0.0f); }
    void setWhite() { glColor3f(1.0f, 1.0f, 1.0f); }

    void setColor(const RGB &rgb) {
        glColor3f(rgb.r, rgb.g, rgb.b);
    }

    void drawSquare(float x, float y, float r) {
        glRectf(x - r, y - r, x + r, y + r);
    }

    void drawSquareBorder(float x, float y, float r) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(x - r, y - r);
        glVertex2f(x - r, y + r);
        glVertex2f(x + r, y + r);
        glVertex2f(x + r, y - r);
        glEnd();
    }

    void drawCircle(float x, float y, float r) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= N_SEG; ++i) {
            const float theta = w * i;
            glVertex2f(x + r * cos(theta),
                y + r * sin(theta));
        }
        glEnd();
    }

    void drawCircleBorder(float x, float y, float r) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= N_SEG; ++i) {
            const float theta = w * i;
            glVertex2f(x + r * cos(theta),
                y + r * sin(theta));
        }
        glEnd();
    }
}

void Monitor::draw() {
    using namespace math;
    using namespace gl;
    static const int Lhalf = g.L / 2;
    //static double
        //*Dr = new double[g.N];
        //*SigDr = new double[g.N];
    
    if (!canvas->valid()) {
        glViewport(0, 0, canvas->w(), canvas->h());
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, g.L, 0.0, g.L, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    glLineWidth(1.0f);
    // Background
    if (vcolor == VC_BLACK)
        bgBlack();
    else
        bgWhite();

    // Particles
    if (pcolor == PC_PLAIN) {
        if (vcolor == VC_BLACK)
            setWhite();
        else
            setBlack();
    } 
    //else if (pcolor == PC_DR) {
        //FOR(i, g.N) {
        //    const int
        //        dx = period_dist(X_[i], X__[i], g.L),
        //        dy = period_dist(Y_[i], Y__[i], g.L);
        //    const double dr = sqrt(square(dx) + square(dy));
        //    Dr[i] = dr;
        //}
        //sigmoid(Dr, SigDr, g.N, sigThres, sigTol);
    //}
    FOR(i, g.N) {
        if (pcolor == PC_RAINBOW)
            setColor(rainbow(float(i) / g.N));
        else if (pcolor == PC_DR) {
            const int
                dx = period_dist(X_[i], X__[i], g.L),
                dy = period_dist(Y_[i], Y__[i], g.L);
            float x; // 3 levels
            if (dx == 0 && dy == 0)
                x = 0.0f;
            else if (dx + dy == 1)
                x = 0.5f;
            else
                x = 1.0f;
            if (vcolor == VC_BLACK)
                glColor3f(1.0f, 1.0f - x, 1.0f - x);
            else
                glColor3f(x, 0.0f, 0.0f);
        }
        if (shape == SP_SQUARE)
            drawSquare(X_[i] + 0.5f, Y_[i] + 0.5f, radius);
        else
            drawCircle(X_[i] + 0.5f, Y_[i] + 0.5f, radius);
    }
    // Particle borders
    if (vcolor == VC_WHITEBB) {
        setBlack();
        FOR(i, g.N) {
            if (shape == SP_SQUARE)
                drawSquareBorder(X_[i] + 0.5f, Y_[i] + 0.5f, radius);
            else
                drawCircleBorder(X_[i] + 0.5f, Y_[i] + 0.5f, radius);
        }
    }

    // Auxiliary lines
    glLineWidth(linewidth);
    switch (dispMode) {
    case DM_NONE:
        break;
    case DM_PDISP:
        glBegin(GL_LINES);
        FOR(i, g.N) {
            const int d = abs(X__[i] - X_[i]) + abs(Y__[i] - Y_[i]);
            if (d && d < Lhalf) {
                setColor(rainbow(float(i) / g.N));
                glVertex2f(X__[i] + 0.5f, Y__[i] + 0.5f);
                glVertex2f(X_[i] + 0.5f, Y_[i] + 0.5f);
            }
        }
        glEnd();
        break;
    case DM_VDISP:
        glBegin(GL_LINES);
        FOR2(i, g.N, g.V) {
            const int d = abs(X__[i] - X_[i]) + abs(Y__[i] - Y_[i]);
            if (d && d < Lhalf) {
                setColor(rainbow(float(i - g.N) / g.NV));
                glVertex2f(X__[i] + 0.5f, Y__[i] + 0.5f);
                glVertex2f(X_[i] + 0.5f, Y_[i] + 0.5f);
            }
        }
        glEnd();
        break;
    case DM_TRAJ:
        int *_X = new int[g.NV],
            *_Y = new int[g.NV];
        FOR2(i, g.N, g.V) {
            _X[i - g.N] = X__[i];
            _Y[i - g.N] = Y__[i];
        }
        const int n = int(vvec.size());
        glBegin(GL_LINES);
        FOR(s, n) {
            const int v = vvec[s] - g.N;
            double x1 = _X[v], y1 = _Y[v];
            displacew(_X[v], _Y[v], vdvec[s], g.L, _X[v], _Y[v]);
            double x2 = _X[v], y2 = _Y[v];
            if (abs(x2 - x1) + abs(y2 - y1) < Lhalf) {
                if (deviate) {
                    x1 += rand.randu(-1, 1) * deviate;
                    y1 += rand.randu(-1, 1) * deviate;
                    x2 += rand.randu(-1, 1) * deviate;
                    y2 += rand.randu(-1, 1) * deviate;
                }
                setColor(rainbow(float(v) / g.NV));
                glVertex2d(x1 + 0.5, y1 + 0.5);
                glVertex2d(x2 + 0.5, y2 + 0.5);
            }
        }
        glEnd();
        delete[] _X;
        delete[] _Y;
        break;
    }
    if (!hasRedrawn) {
        if (isCapturing)
            _capture();
        hasRedrawn = true;
    }
}

static void savepng(const char filename[], uint8_t *image, int w, int h) {
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    FILE *fp = fopen(filename, "wb");
    png_init_io(png, fp);
    png_set_IHDR(png, info, w, h, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_colorp palette = png_colorp(png_malloc(png, PNG_MAX_PALETTE_LENGTH * sizeof(png_color)));
    png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
    png_write_info(png, info);
    png_set_packing(png);
    png_bytepp rows = png_bytepp(png_malloc(png, h * sizeof(png_bytep)));
    for (int i = 0; i < h; ++i) {
        rows[i] = image + (h - 1 - i) * w * 3;
        //rows[i] = (png_bytep) image + i * w * 3;
    }
    png_write_image(png, rows);
    png_write_end(png, info);
    png_free(png, rows);
    png_free(png, palette);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

void Monitor::_capture() const {
    using namespace std;
    static int iframe = 0;
    if (++iframe > 9999) {
        PRINTLN("ERROR: Too many frames");
        exit(EXIT_FAILURE);
    }
    const int w = canvas->w(), h = canvas->h();
    uint8_t *image = new uint8_t[w * h * 3];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, static_cast<void *>(image));
    //fl_read_image(image, x, y, w, h);
    char buf[BUF];
    snprintf(buf, BUF, "%05d", iframe);
    if (isCapturing)
        btnCapture->copy_label(buf);
    snprintf(buf, BUF, "%05d.png", iframe);
    savepng(buf, image, w, h);
    cout << "Capture [" << iframe << "]: " << g.get_mcs() << '\t' << g.get_time() << endl;
    delete[] image;
}
