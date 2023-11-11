#pragma once
#include <math/vector.h>
#include <math/quarternion.h>
// #include <math/filter.h>
// #include <math/matrix.h>

#include <string>

namespace serialize
{
    /**
     * @brief Used to serialize vectors to send using libmissioncontrol
     * 
     * @param v 
     * @return std::string 
     */
    std::string serialize(const math::vector& v);

    std::string serialize(const math::quarternion& q);
} // namespace serialize