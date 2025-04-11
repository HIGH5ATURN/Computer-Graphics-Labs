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
   Light(const Vec3f& pos, const Vec3f& amb, const Vec3f diff, const Vec3f spec) 
	   : position(pos), ambient(amb), diffuse(diff), specular(spec) {}

};

#endif