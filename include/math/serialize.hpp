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
    std::string serialize(const math::vector& v) {
        return "{\"x\":"+std::to_string(v.x)+",\"y\":"+std::to_string(v.y)+",\"z\":"+std::to_string(v.z)+"}";
    }

    std::string serialize(const math::quarternion& q) {
        return "{\"w\": "+std::to_string(q.w)+",\"x\":"+std::to_string(q.x)+",\"y\":"+std::to_string(q.y)+",\"z\":"+std::to_string(q.z)+"}";
    }
} // namespace serialize