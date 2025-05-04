#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"
#include "Material.h"

class Triangle : public Object {
public:
    Vec3f v0, v1, v2;
    Vec3f normal;
    const Material* material;
    bool flipped;
    Triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Material* mat, bool flip) : 
        v0(a), v1(b), v2(c), material(mat), flipped(flip=false)
    {
        // Normal calculation from the cross product of two edges of the triangle
        normal = ((v1 - v0).cross(v2 - v0)).normalize();

    }


    bool hit(const Ray& r, HitRec& rec) const;
    
    const Material* getMaterial() const;

    void computeSurfaceHitFields(const Ray& ray, HitRec& rec) const;


   
};

#endif
