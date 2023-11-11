#include <math/filter_vector.h>

math::vector filter::filter_vector::operator [](const math::vector& x0) {
    math::vector y0 = x0 * b0 + x1 * b1 + x2 * b2 + y1 * a1 + y2 * a2;
    x2 = x1;
    x1 = x0;
    y2 = y1;
    y1 = y0;
    return y0;
}
    
void filter::filter_vector::operator=(const filter& base) {
    b0 = base.b0;
    b1 = base.b1;
    b2 = base.b2;
    a1 = base.a1;
    a2 = base.a2;
}