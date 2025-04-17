#ifndef _HITREC_H_
#define _HITREC_H_

#include "Vec3.h"

class Material; // Forward declaration

struct HitRec {
    float tHit;
    int primIndex;
    Vec3f p;
    Vec3f n;
    bool anyHit;
    const Material* material;
};

#endif
