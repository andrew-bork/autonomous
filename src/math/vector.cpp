#include <math/vector.h>

math::vector::vector (){
    x = y = z = 0;
}
math::vector::vector (double xa){
    x = xa;
    y = z = 0;
}
math::vector::vector (double xa, double ya){
    x = xa;
    y = ya;
    z = 0;
}
math::vector::vector (double xa, double ya, double za){
    x = xa;
    y = ya;
    z = za;
}

math::vector math::vector::operator+ (const math::vector& r){
    vector res(x+r.x,y+r.y,z+r.z);
    return res;
}

math::vector math::vector::operator*(const double& s){
    vector res(x*s, y*s, z*s);
    return res;
}
