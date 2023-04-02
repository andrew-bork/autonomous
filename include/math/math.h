#ifndef MATH_H
#define MATH_H

#define DEG_TO_RAD 0.017453292519943
#define RAD_TO_DEG 57.29577951308232

#include <string>
namespace math{

    struct vector{
        double x,y,z;
        vector();
        vector(double x);
        vector(double x, double y);
        vector(double x, double y, double z);

        vector operator+ (const vector& r);
        vector operator* (const double& s);
    };

    struct quarternion{
        double w, x, y, z;
        bool unit;

        quarternion();
        quarternion(double x, double y, double z);
        quarternion(double w, double x, double y, double z);
        quarternion(double w, double x, double y, double z, bool unit);

        quarternion operator+ (const quarternion& r);
        quarternion operator* (const quarternion& r);

        static quarternion inverse(const quarternion& n);
        static quarternion conjugate(const quarternion& n);
        static quarternion rotate(double theta, const vector& axis);
        static quarternion fromEulerZYX(const vector& euler);
        
        static vector toEuler(const quarternion& q);
        static vector toMagAxis(const quarternion& q);
        static vector rotateVector(math::quarternion& q, math::vector& in);
    };



    double length(const quarternion& n);
    double length(const vector& n);

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
}
#endif