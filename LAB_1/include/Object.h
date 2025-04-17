#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Ray.h"
#include "HitRec.h"

class Material;

class Object {
    private:
public:
    virtual ~Object() {};
    virtual bool hit(const Ray & r, HitRec & rec) const = 0;

    virtual const Material* getMaterial() const=0;

    virtual void computeSurfaceHitFields(const Ray& r, HitRec& rec) const=0;
};

#endif