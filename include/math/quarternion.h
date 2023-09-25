#ifndef QUARTERNION_H
#define QUARTERNION_H



#include <math/vector.h>

namespace math {
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
        static quarternion from_euler_ZYX(const vector& euler);
        
        static vector to_euler(const quarternion& q);
        static vector to_magnitude_axis(const quarternion& q);
        static vector rotate_vector(math::quarternion& q, math::vector& in);
    };
    
    double length(const quarternion& n);

};

#endif