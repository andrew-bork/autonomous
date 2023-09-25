#ifndef VECTOR_H
#define VECTOR_H

namespace math{
    struct vector{
        double x,y,z;
        vector();
        vector(double x);
        vector(double x, double y);
        vector(double x, double y, double z);

        vector operator+ (const vector& r);
        vector operator* (const double& s);

        double dot(const vector& r) const;
    };

    double length(const vector& n);
};

#endif