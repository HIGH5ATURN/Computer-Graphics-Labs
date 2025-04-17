#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Vec3.h"

class Material {
public:
	Vec3f ambientColor;
	Vec3f diffuseColor;
	Vec3f specularColor;
	float shineness;
	float reflectivity;
	float fuzziness;
	float transparency;     // 0 = opaque, 1 = fully transparent
	float refractiveIndex;  // e.g., 1.0 (air), 1.33 (water), 1.5 (glass)
	
public:
	Material(const Vec3f& ambient, const Vec3f& diffuse, const Vec3f& specular, const float shine, float reflect = 0.0f,float fuzz=0.0f, float trans=0.0f , float refract=1.0f)
		: ambientColor(ambient), 
		diffuseColor(diffuse), 
		specularColor(specular), 
		shineness(shine), 
		reflectivity(reflect), 
		fuzziness(fuzz),
		transparency(trans),
		refractiveIndex(refract){
	}
};
//	Material(const Vec3f& ambient, const Vec3f& diffuse, const Vec3f& specular, const float shine)
//		: ambientColor(ambient), diffuseColor(diffuse), specularColor(specular), shineness(shine) {
//	}
//};


#endif
