#ifndef _PLANE_H_
#define _PLANE_H_

#include "Vec3.h"
#include "Ray.h"
#include "Object.h"
#include "Material.h"

class Plane : public Object {
public:
    Vec3f point;              
    Vec3f normal;              // The normal vector of the plane (should be normalized)
    const Material* material; 

    Plane(const Vec3f& pt, const Vec3f& n, const Material* mat)
        : point(pt), material(mat) {
        Vec3f nr = n;
        normal = nr.normalize();
    }

    bool hit(const Ray& ray, HitRec& rec) const override {
        float denom = normal.dot(ray.d);

        // If denom is close to 0, ray is parallel to the plane
        if (fabs(denom) > 1e-6f) {
            // Plane equation: n • p = -D
            float D = -(normal.dot(point));
            // t= -(D+n.o)/n.d
            float t = -(D + normal.dot(ray.o)) / denom;

            if (t > 0 && t < rec.tHit && t < ray.tClip) {
                rec.tHit = t;
                rec.p = ray.o + ray.d * t;
                rec.n = normal;
                rec.material = material;
                rec.anyHit = true;
                return true;
            }
        }

        return false;
    }



};

#endif
