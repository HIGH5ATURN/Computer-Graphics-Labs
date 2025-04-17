#include "Triangle.h"



bool Triangle::hit(const Ray& r, HitRec& rec) const {
    float denom = normal.dot(r.d);
    if (fabs(denom) < 1e-6) return false;

    float D = -(normal.dot(v0));
    float t = -(D + normal.dot(r.o)) / denom;

    if (t < 1e-4 || t >= r.tClip) return false;

    Vec3f p = r.o + r.d * t;

    Vec3f c0 = (v1 - v0).cross(p - v0);
    Vec3f c1 = (v2 - v1).cross(p - v1);
    Vec3f c2 = (v0 - v2).cross(p - v2);

    if ((normal.dot(c0) >= 0) && (normal.dot(c1) >= 0) && (normal.dot(c2) >= 0)) {
        rec.tHit = t;
        rec.anyHit = true;
        return true;
    }
    return false;
}

const Material* Triangle::getMaterial() const {

    return this->material;
}
void Triangle::computeSurfaceHitFields(const Ray& ray, HitRec& rec) const {
    rec.p = ray.o + ray.d * rec.tHit;  // intersection point
    rec.n = normal;                    // precomputed triangle normal
}

