#include "RayTracer.h"
#include "cmath"
#include "stdio.h"
#include "raytrace_features.h"
using namespace std;
void RayTracer::searchClosestHit(const Ray & ray, HitRec & hitRec) {
    hitRec.anyHit = false;

    hitRec.tHit = ray.tClip;

    bool hit = false;
    for (int i = 0; i < this->scene->objects.size(); i++) {
        hit = false;
       hit=this->scene->objects[i]->hit(ray, hitRec);
       if (hit) {
           hitRec.primIndex = i;
       }
    }


    //printf("%f\n", minHitRec.tHit);
}

Vec3f RayTracer::getEyeRayDirection(int x, int y) {
    //Uses a fix camera looking along the negative z-axis
    static float z = -5.0f;		
    static float sizeX = 4.0f; 
    static float sizeY = 3.0f; 
    static float left = -sizeX * 0.5f;
    static float top = sizeY * 0.5f;
    static float dx =  sizeX / float(this->image->getWidth());  
    static float dy =  sizeY / float(this->image->getHeight());

    return Vec3f((left + x * dx), (top - y * dy), z).normalize();
}

void RayTracer::fireRays() {
    Ray ray;

    for(int x = 0; x < this->image->getWidth(); x++) {
        for(int y = 0; y < this->image->getHeight(); y++) {
            ray.d = RayTracer::getEyeRayDirection(x, y);
			
            RayTracer::searchClosestHit(ray, ray.hitRec);

            if (ray.hitRec.anyHit) {

                Vec3f color = Vec3f(0.0f, 0.0f, 0.0f);
                for (int i = 0;i < this->scene->lights.size();i++) {
                    //color += this->scene->lights[i]->ambient.multCoordwise(ray.hitRec.color);

                    color += computeLightColor(ray, ray.hitRec, this->scene->lights[i]);
                }

                this->image->setPixel(x, y, color);
            }
            else {

                //Now I also need to adjust the light for the surrounding here!
                //To do so we need to do add the ambient lights and stuff with this 
                //so make another function that will compute light only based on color

                this->image->setPixel(x, y, this->bgColor);

            }
        }
    }
}

Vec3f RayTracer::computeLightColor(const Ray& ray, HitRec& hitRec, const Light* light) {
    Vec3f N = hitRec.n.normalize();                                 // Surface normal
    Vec3f L = (light->position - hitRec.p).normalize();             // Light direction

    Vec3f ambient = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f diffuse = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f specular = Vec3f(0.0f, 0.0f, 0.0f);

#if defined(AMBIENT_LIGHTING)
    ambient = hitRec.material->ambientColor.multCoordwise(light->ambient);
#endif

    bool inShadow = false;

#if defined(SHADOWS_BLACK) || defined(SHADOWS_AMBIENT)
    Vec3f shadowOrigin = hitRec.p + N * 1e-4f; // Small offset to avoid self-hit
    Ray shadowRay(shadowOrigin, L);
    shadowRay.tClip = (light->position - shadowOrigin).len(); // Stop at light

    HitRec shadowHitRec;
    searchClosestHit(shadowRay, shadowHitRec);
    if (shadowHitRec.anyHit && shadowHitRec.tHit < shadowRay.tClip) {
        inShadow = true;
    }
#endif

    bool skipLightCalc = false;
#if defined(SHADOWS_BLACK)
    if (inShadow) {
        skipLightCalc = true;
        ambient = Vec3f(0.0f, 0.0f, 0.0f);
    }
#elif defined(SHADOWS_AMBIENT)
    if (inShadow) {

        skipLightCalc = true;
    }
#endif

    if (!skipLightCalc) {
#if defined(DIFFUSE_LIGHTING)
        float diff = std::max(0.0f, N.dot(L));
        diffuse = hitRec.material->diffuseColor.multCoordwise(light->diffuse) * diff;
#endif

#if defined(SPECULAR_LIGHTING)
        Vec3f V = (ray.o - hitRec.p).normalize();                       // View direction
        Vec3f R = (N * (2.0f * N.dot(L)) - L).normalize();              // Reflection direction

        float spec = pow(std::max(0.0f, R.dot(V)), hitRec.material->shineness);
        specular = hitRec.material->specularColor.multCoordwise(light->specular) * spec;
#endif
    }

    // Combine components
    Vec3f color = ambient + diffuse + specular;

    // Clamp color to [0, 1]
    color.x = std::min(color.x, 1.0f);
    color.y = std::min(color.y, 1.0f);
    color.z = std::min(color.z, 1.0f);

    return color;
}
