#ifndef __HitRecord__
#define __HitRecord__

#include "Vec3.hpp"  
#include "Point3.hpp"
#include "Normal3.hpp"     
#include "Vec2.hpp"       
#include "Ray.hpp"        
//#include "material.hpp"   Material
#include <memory>       // std::shared_ptr
#include <cmath>        // std::fabs

class HitRecord {
public:
    Point3 worldpoint;              
    Normal3 normal;                   
    Vec2 surfacepoint;            
    double t;                       
    Ray ray;                       

    HitRecord() = default;

    bool isClose(const HitRecord& other, float epsilon = 1e-5f) {
        return worldpoint.isClose(other.worldpoint, epsilon) &&
               normal.isClose(other.normal, epsilon) &&
               surfacepoint.isClose(other.surfacepoint, epsilon) &&
               (std::abs(t - other.t) < epsilon) &&
               ray.isClose(other.ray, epsilon);
    }
    
};

#endif
