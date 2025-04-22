#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Vec3.h"
#include "Object.h"

class Light {
public:
    Vec3f position;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;

    // --- New Soft Shadow Support ---
    float radius;      // Radius for soft shadow (area light)
    int samples;       // Number of shadow rays for soft shadow sampling

    Light(const Vec3f& pos, const Vec3f& amb, const Vec3f& diff, const Vec3f& spec,
        float rad = 0.0f, int samp = 1)
        : position(pos), ambient(amb), diffuse(diff), specular(spec), radius(rad), samples(samp) {
    }
};

#endif
