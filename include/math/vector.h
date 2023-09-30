#ifndef VECTOR_H
#define VECTOR_H

namespace math{
    struct vector{
        double x,y,z;
        /**
         * @brief Construct a zero vector
         * 
         */
        vector();
        /**
         * @brief Construct a vector with an x component
         * 
         * @param x 
         */
        vector(double x);
        /**
         * @brief Construct a vector with an x and y component
         * 
         * @param x 
         * @param y 
         */
        vector(double x, double y);
        /**
         * @brief Construct a new vector with an x, y, and z component
         * 
         * @param x 
         * @param y 
         * @param z 
         */
        vector(double x, double y, double z);
        
        /**
         * @brief Vector addition
         * 
         * @param r 
         * @return vector 
         */
        vector operator+ (const vector& r) const;

        /**
         * @brief Vector scaling
         * 
         * @param s 
         * @return vector 
         */
        vector operator* (const double& s) const;

        /**
         * @brief Vector subtraction
         * 
         * @param r 
         * @return vector 
         */
        vector operator- (const vector& r) const;

        /**
         * @brief Dot product
         * 
         * @param r 
         * @return double 
         */
        double dot(const vector& r) const;
    };

    double length(const vector& n);
};

#endif