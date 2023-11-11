#pragma once

#include <math/vector.h>
#include <math/filter.h>

namespace filter {
    struct filter_vector {
        double a1=0, a2=0, b0=1, b1=0, b2=0;
        math::vector x1=0, x2=0, y1=0, y2=0;
        math::vector operator [](const math::vector& inp);
    
        void operator=(const filter& base);
    };
};