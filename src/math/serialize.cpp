#include <math/serialize.hpp>

std::string serialize::serialize(const math::vector& v) {
    // std::printf("123\n");
    return "{\"x\":"+std::to_string(v.x)+",\"y\":"+std::to_string(v.y)+",\"z\":"+std::to_string(v.z)+"}";
}

std::string serialize::serialize(const math::quarternion& q) {
    // std::printf("abc\n");
    return "{\"w\": "+std::to_string(q.w)+",\"x\":"+std::to_string(q.x)+",\"y\":"+std::to_string(q.y)+",\"z\":"+std::to_string(q.z)+"}";
}