#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include "png.h"
namespace py = pybind11;

double add(double x, double y) {
    return x + y;
}


int read_png(const std::string& file) {
    FILE* infile;
    infile = fopen(file.c_str(), "r");
    unsigned char sig[8];

    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */
    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8))
        return 1;
        //exit(1);   /* bad signature */
    return 0;
}

PYBIND11_MODULE(monkey3, m) {
    m.doc() = "prova prova2"; // optional module docstring
    m.def("add", &add);
    m.def("read", &read_png);
}
