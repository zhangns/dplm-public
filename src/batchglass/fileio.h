#ifndef _FILEIO_H
#define _FILEIO_H
#include <fstream>
#include "utils.h"

namespace fileio {

    bool file_exists(const char filename[]);
    void mkdirx(const char path[]);

    template <class T>
    void put_r(std::ofstream &fout, int n, const T *X, const T *Y) {
        FOR(i, n)
            fout << X[i] << ' ' << Y[i] << NEWLINE;
        fout << ENDL;
    }

    void save_r(const char filename[], int n, const int *X, const int *Y) ;
    void load_r(const char filename[], int n, int *X, int *Y) ;

    template <class T>
    void save_var(const char filename[], T v) {
        std::ofstream fout(filename, std::ofstream::binary);
        fout << v << ENDL;
    }

    template <class T>
    void load_var(const char filename[], T &v) {
        std::ifstream fout(filename);
        fout >> v;
    }

}

#endif
