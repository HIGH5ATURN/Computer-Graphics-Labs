#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Vec3.h"

class Material {
public:
	Vec3f ambientColor;
	Vec3f diffuseColor;
	Vec3f specularColor;
	float shineness;

public:
	Material(const Vec3f& ambient, const Vec3f& diffuse, const Vec3f& specular, const float shine)
		: ambientColor(ambient), diffuseColor(diffuse), specularColor(specular), shineness(shine) {
	}
};

#endif
