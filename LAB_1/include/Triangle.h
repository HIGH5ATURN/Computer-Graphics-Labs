#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"
#include "Material.h"

class Triangle : public Object {
public:
    Vec3f v0, v1, v2;
    Vec3f normal;
    const Material* material;

    Triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Material* mat) : v0(a), v1(b), v2(c), material(mat)
    {
        normal = ((v1 - v0).cross(v2 - v0)).getNormalized();
    }

    bool hit(const Ray& r, HitRec& rec) const;
    
    const Material* getMaterial() const;

    void computeSurfaceHitFields(const Ray& ray, HitRec& rec) const;


   
};

#endif
