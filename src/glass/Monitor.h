#ifndef _MONITOR_H
#define _MONITOR_H
#include <thread>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include "Glass2D.h"
#include "Rand.h"

class Monitor {
public:
    // Starts UI thread
    Monitor(Glass2D &g, const Config &c);
    ~Monitor();
    static Monitor *getMonitor(); // Singleton

    bool notify(); // Notify MCS. Returns whether should terminate
    void end();

private:
    typedef uint8_t hopd_t;
    enum DispMode { DM_NONE, DM_TRAJ, DM_PDISP, DM_VDISP};
    enum Shape    { SP_SQUARE, SP_CIRCLE };
    enum PColor   { PC_PLAIN, PC_RAINBOW, PC_DR };
    enum VColor   { VC_BLACK, VC_WHITE, VC_WHITEBB };

    class Window : public Fl_Window {
    public:
        Window(int w, int h, const char title[]);
        void resize(int X, int Y, int W, int H) override;
    };

    class Canvas : public Fl_Gl_Window {
    public:
        Canvas(int X, int Y, int W, int H);
    protected:
        void draw() override;
    };

    static Monitor *psMonitor;
    const Config &c;
    Glass2D &g;
    Rand rand;

    double
        vdt, vtimer = 0.0;
    int delay = 0;
    int dispMode = DM_NONE,
        shape = SP_CIRCLE,
        pcolor = PC_RAINBOW,
        vcolor = VC_BLACK;
    float
        radius = 0.50f,
        linewidth = 1.0f,
        deviate = 0.00f;
    double
        sigThres = 0.50,
        sigTol = 0.20;
    bool isCapturing = false;

    // Analysis vars
    int *const X_,          // Last snapshot
        *const Y_,
        *const X__,         // Next-to-last
        *const Y__;
    int nHop = 0, 
        nReturn = 0,
        *const Hopd;        // Last hop direction
    std::vector<int> vvec;
    std::vector<hopd_t> vdvec;

    // Multithreading
    std::thread *flThread;
    volatile bool
        isFlReady  = false,
        hasRedrawn = true,
        isClosing  = false,
        isPaused   = false;

    void draw();
    void onResize();
    void _capture() const;

    // GUI    
    Window *window;
    Canvas *canvas;
    Fl_Simple_Counter
        *cntVdt, *cntVdtScale, *cntDelay, *cntRadius,
        *cntLinewidth, *cntDeviate,
        *cntSigThres, *cntSigTol;
    Fl_Spinner
        *spinT;
    Fl_Toggle_Button
        *btnPause, *btnCapture;
    Fl_Box
        *boxMCS, *lblMCS,
        *boxTime, *lblTime,
        *lblT,
        *boxE, *lblE,
        *boxMSD, *lblMSD,
        *boxPret, *lblPret,
        *boxDxMax, *lblDxMax,
        *boxSize, *lblSize,
        *lblVdt, *lblDelay,
        *lblRadius, *lblLinewidth, *lblDeviate,
        *lblSigmoid,
        *lblShape, *lblParticle, *lblVoid,
        *lblDisplay;
    Fl_Group
        *pGrpShape, *pGrpParticle, *pGrpVoid;
    Fl_Radio_Round_Button
        *radShapeSquare, *radShapeCircle,
        *radPPlain, *radPRainbow, *radPDr,
        *radVoidBlack, *radVoidWhite, *radVoidWhiteBB,
        *radDispNone, *radDispTraj, *radDispPDisp, *radDispVDisp;

    void _run();
    void setDispMode(int dispMode);
    void setShape(int shape);
    void setPcolor(int pcolor);
    void setVcolor(int vcolor);
    void onSetVdt();
    void onSetT(); double newT = -1;
    void onSetDelay();
    void onSetRadius();
    void onSetLinewidth();
    void onSetDeviate();
    void onSetSigThres();
    void onSetSigTol();
    void onPause();
    void onCapture();
    void callback();
    void _exit();

    friend void flWorker();
    friend void radDisplay_cb(Fl_Widget *, long v);
    friend void radShape_cb(Fl_Widget *, long v);
    friend void radPcolor_cb(Fl_Widget *, long v);
    friend void radVcolor_cb(Fl_Widget *, long v);
    friend void cntVdt_cb(Fl_Widget *);
    friend void spinT_cb(Fl_Widget *);
    friend void cntDelay_cb(Fl_Widget *);
    friend void cntRadius_cb(Fl_Widget *);
    friend void cntLinewidth_cb(Fl_Widget *);
    friend void cntDeviate_cb(Fl_Widget *);
    friend void cntSigThres_cb(Fl_Widget *);
    friend void cntSigTol_cb(Fl_Widget *);
    friend void btnPause_cb(Fl_Widget *);
    friend void btnCapture_cb(Fl_Widget *);
    friend void window_cb(Fl_Widget *, void *);
    friend void exit_msg_handler(void *);
};

#endif
