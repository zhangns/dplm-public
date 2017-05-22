#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "fileio.h"
using namespace std;

bool fileio::file_exists(const char filename[]) {
    return !!ifstream(filename);
}

void fileio::mkdirx(const char path[]) {
    char cmd[BUF] = {};
#if _WIN32
    char buf[BUF] = {};
    strncpy(buf, path, BUF);
    FOR(i, BUF)
        if (buf[i] == '/')
            buf[i] = '\\';
    snprintf(cmd, BUF, "md %s", buf);
#else
    snprintf(cmd, BUF, "mkdir -p %s", path);
#endif
    system(cmd);
}

void fileio::save_r(const char filename[], int n, const int *X, const int *Y) {
    ofstream fout(filename, ofstream::binary);
    put_r(fout, n, X, Y);
}

void fileio::load_r(const char filename[], int n, int *X, int *Y) {
    ifstream fin(filename);
    FOR(i, n)
        fin >> X[i] >> Y[i];
}
