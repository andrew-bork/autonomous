#ifndef MATRIX_H
#define MATRIX_H

#include <string>

namespace math {
    struct matrix {
        double * array = nullptr;
        unsigned int r, c;

        matrix();
        matrix(const unsigned int r,const unsigned int c);
        matrix(const double * array, const unsigned int r, const unsigned int c);
        ~matrix();

        void setup(const unsigned int r,const unsigned int c);
        void setup(const double * array, const unsigned int r, const unsigned int c);

        static void multiply(const matrix& r, const matrix& l, matrix& out);
        static void add(const matrix& r, const matrix& l, matrix& out);
        static void inverse(const matrix& a, matrix& out);
        static void transpose(const matrix& a, matrix& out);

        static std::string string(const matrix& a);

        void set(double value, int _r, int _c);
        // double get();
        // void get(double * data, const std::pair<int, int> upper, const std::pair<int,int> lower);
        // void set(double data, const std::pair<int, int> loc);
        // void set(double * data, const std::pair<int, int> upper, const std::pair<int,int> lower);

        matrix operator+ (const matrix& r);
        matrix operator* (const matrix& r);
    };
};

#endif