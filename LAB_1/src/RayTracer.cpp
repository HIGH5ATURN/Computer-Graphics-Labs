#include "RayTracer.h"
#include "cmath"
#include "stdio.h"
#include "raytrace_features.h"
using namespace std;
void RayTracer::searchClosestHit(const Ray & ray, HitRec & hitRec) {
    hitRec.anyHit = false;

    hitRec.tHit = ray.tClip;
    int x=0;
    bool hit = false;
    for (int i = 0; i < this->scene->objects.size(); i++) {
        hit = false;
       hit=this->scene->objects[i]->hit(ray, hitRec);
       if (hit) {
          x = i;
       }
    }

    //More optimized --> now the algorithm finds the closest hit first.
    //then calculates the rest of the needs
    hitRec.primIndex = x;

    hitRec.material = this->scene->objects[hitRec.primIndex]->getMaterial();

    this->scene->objects[hitRec.primIndex]->computeSurfaceHitFields(ray,hitRec);
   
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
            //printf("pixel: %d, %d \n", x,y);
            ray.d = RayTracer::getEyeRayDirection(x, y);
		
            Vec3f  color = traceRay(ray, 5);
            this->image->setPixel(x, y, color);

        }
    }
}




Vec3f RayTracer::traceRay(const Ray& ray, int depth) {
    if (depth <= 0) return bgColor; // Base case for recursion

    HitRec hitRec;
    searchClosestHit(ray, hitRec);

    if (hitRec.anyHit) {



        Vec3f localColor = Vec3f(0.0f, 0.0f, 0.0f);
        Vec3f reflectedColor = Vec3f(0.0f, 0.0f, 0.0f);
        bool inShadow = false;
        // Compute direct lighting
        for (const Light* light : scene->lights) {
            localColor += computeLightColor(ray, hitRec, light, inShadow);
        }
        

        if (inShadow) {
            return localColor;
        }
    #if defined REFLECTIONS
       
        // Compute reflections if material is reflective
        if (hitRec.material->reflectivity > 0.0f) {


            Vec3f N = hitRec.n;
            Vec3f reflectionDir = ray.d - N*( 2.0f * (ray.d.dot(N)));
            
            
            // Applying fuzziness
            float fuzz = hitRec.material->fuzziness;
            if (fuzz > 0.0f) {   
                Vec3f randomVec = RayTracer::randomInUnitSphere(); 
                reflectionDir = (reflectionDir + randomVec * fuzz).normalize();
            }


            Ray reflectionRay(hitRec.p + N * Ray::rayEps, reflectionDir);

            // Create reflection ray with small offset to avoid self-intersection

             reflectedColor = traceRay(reflectionRay, depth - 1);

            // Blend between surface color and reflected color based on reflectivity
           
        }
        #endif
        Vec3f finalColor = localColor * (1.0f - hitRec.material->reflectivity) +
            reflectedColor * hitRec.material->reflectivity;
        return finalColor;
    }
    else {
        // Background color
        return bgColor;
    }
}





Vec3f RayTracer::computeLightColor(const Ray& ray, HitRec& hitRec, const Light* light, bool & shadow) {
    Vec3f N = hitRec.n;                                 // Surface normal
    Vec3f L = (light->position - hitRec.p).normalize();             // Light direction
    Vec3f localColor = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f ambient = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f diffuse = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f specular = Vec3f(0.0f, 0.0f, 0.0f);

#if defined(AMBIENT_LIGHTING)
    ambient = hitRec.material->ambientColor.multCoordwise(light->ambient);
#endif

    bool inShadow = false;

#if defined(SHADOWS_BLACK) || defined(SHADOWS_AMBIENT)
    Vec3f shadowOrigin = hitRec.p + N * Ray::rayEps; // Small offset to avoid self-hit
    Ray shadowRay(shadowOrigin, L);
    shadowRay.tClip = (light->position - shadowOrigin).len(); // Stop at light

    HitRec shadowHitRec;

    searchClosestHit(shadowRay, shadowHitRec);

    if (shadowHitRec.anyHit && shadowHitRec.tHit < shadowRay.tClip) {
        inShadow = true;
    }
#endif
   
    shadow = inShadow;
    if (inShadow) {
    #if defined(SHADOWS_BLACK)
            return Vec3f(0.0f, 0.0f, 0.0f);
    #elif defined(SHADOWS_AMBIENT)
            return ambient;
    #endif
    }


   
#if defined(DIFFUSE_LIGHTING)
        float diff = std::max(0.0f, N.dot(L));
        diffuse = hitRec.material->diffuseColor.multCoordwise(light->diffuse) * diff;
#endif

#if defined(SPECULAR_LIGHTING)
        Vec3f V = -ray.d;                    
        Vec3f R = (N * (2.0f * N.dot(L)) - L).normalize();          
        float angle = max(0.0f, R.dot(V));
        float spec = pow(angle, hitRec.material->shineness);
        specular = hitRec.material->specularColor * spec;
#endif
    

    localColor = ambient + diffuse + specular;
    // Clamp color to [0, 1]
    localColor.x = min(max(0.0f, localColor.x), 1.0f);
    localColor.y = min(max(0.0f, localColor.y), 1.0f);
    localColor.z = min(max(0.0f, localColor.z), 1.0f);

    return localColor;
}


