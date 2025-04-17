#include<Plane.h>

bool Plane::hit(const Ray& ray, HitRec& rec) const {
    float denom = normal.dot(ray.d);

    // If denom is close to 0, ray is parallel to the plane
    if (fabs(denom) > 1e-6f) {
        // Plane equation: n • p = -D
        float D = -(normal.dot(point));
        // t= -(D+n.o)/n.d
        float t = -(D + normal.dot(ray.o)) / denom;

        if (t > 0 && t < rec.tHit && t < ray.tClip) {
            rec.tHit = t;
           
            rec.anyHit = true;
            return true;
        }
    }

    return false;
}

const Material* Plane::getMaterial() const {

    return this->material;
}

void  Plane::computeSurfaceHitFields(const Ray& r, HitRec& rec) const {
    rec.p = r.o + r.d * rec.tHit;
    rec.n = normal;
}
