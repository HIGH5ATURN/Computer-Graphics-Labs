#include "RayTracer.h"
#include "cmath"
#include "stdio.h"
#include "raytrace_features.h"
#include <vector>
#include <thread>
#include <mutex>
#include <random>

std::mutex imageMutex;
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
    int samples = 8;  // Number of samples for anti-aliasing

    // Random number generation for jittering (better randomness)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);

    ray.o = Vec3f(-2.0f, 2.0f, 5.0f);

    for (int x = 0; x < this->image->getWidth(); x++) {
        for (int y = 0; y < this->image->getHeight(); y++) {
#ifdef ANTI_ALIASING
            Vec3f accumulatedColor(0.0f, 0.0f, 0.0f);

            // Supersampling: shoot multiple rays per pixel
            for (int i = 0; i < samples; i++) {
               
                float offsetX = dis(gen);
                float offsetY = dis(gen);

              
                ray.d = getEyeRayDirection(x + offsetX, y + offsetY);

                
                accumulatedColor += traceRay(ray, 5);  
            }

           
            accumulatedColor *= (1.0f / float(samples));

            
            this->image->setPixel(x, y, accumulatedColor.clamp());  
#else
         
            ray.d = getEyeRayDirection(x, y);
            Vec3f color = traceRay(ray, 5);  
            this->image->setPixel(x, y, color);
#endif
        }
    }
}



Vec3f RayTracer::calculateRefraction(const Ray& ray, HitRec& hitRec, int depth) {
    Vec3f N = hitRec.n;
    float eta = hitRec.material->refractiveIndex;
    float cosi = -std::max(-1.0f, std::min(1.0f, ray.d.dot(N)));

    float etai = 1.0f, etat = eta;
    if (cosi < 0.0f) {
        cosi = -cosi;
        std::swap(etai, etat);
        N = -N;
    }

    float etaRatio = etai / etat;
    float k = 1.0f - etaRatio * etaRatio * (1.0f - cosi * cosi);

    if (k < 0.0f) {
        // Total internal reflection
        return Vec3f(0.0f,0.0f,0.0f);
    }
    else {
        Vec3f refractDir = (ray.d * etaRatio + N * (etaRatio * cosi - sqrtf(k))).normalize();
        Ray refractedRay(hitRec.p - N * Ray::rayEps, refractDir); 
        return traceRay(refractedRay, depth - 1);
    }
}


Vec3f RayTracer::traceRay(const Ray& ray, int depth) {
    if (depth <= 0) return bgColor;

    HitRec hitRec;
    searchClosestHit(ray, hitRec);

    if (hitRec.anyHit) {
        Vec3f localColor(0.0f, 0.0f, 0.0f),
            reflectedColor(0.0f, 0.0f, 0.0f),
            refractedColor(0.0f, 0.0f, 0.0f);
        bool inShadow = false;

        for (const Light* light : scene->lights) {
            localColor += computeLightColor(ray, hitRec, light, inShadow);
        }



       
    #if defined REFLECTIONS
            if (hitRec.material->reflectivity > 0.0f)
                reflectedColor = calculateReflection(ray, hitRec, depth);
    #endif

    #if defined REFRACTIONS
            if (hitRec.material->transparency > 0.0f)
                refractedColor = calculateRefraction(ray, hitRec, depth);
    #endif

        // Combine all
            float R = 0.0f;
            float T = 0.0f;
            
    #ifdef REFLECTIONS   
            R=hitRec.material->reflectivity;
    #endif
    #ifdef REFRACTIONS
            T = hitRec.material->transparency;
           
    #endif 

      
       float base = 1.0f - R - T;
        Vec3f finalColor = localColor * base + reflectedColor * R + refractedColor * T;
        return finalColor.clamp();
    }

    return bgColor2;
}




Vec3f RayTracer:: calculateReflection(const Ray& ray, HitRec& hitRec, int depth){
    // Compute reflections if material is reflective
    if (hitRec.material->reflectivity > 0.0f) {

        Vec3f N = hitRec.n;
        Vec3f reflectionDir = ray.d - N * (2.0f * (ray.d.dot(N)));

        /*---------Pertrub Normal HERE-----------*/
        #ifdef FUZZY_NORMALS
                reflectionDir = randomlyPerturbNormals(reflectionDir, hitRec.material);
        #endif

        Ray reflectionRay(hitRec.p + N * Ray::rayEps, reflectionDir);

        

       Vec3f reflectedColor = traceRay(reflectionRay, depth - 1);

       
    }
    else {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
}

Vec3f RayTracer:: randomlyPerturbNormals(Vec3f reflectionDir, const Material* mat) {
    // Applying fuzziness
    float fuzz = mat->fuzziness;
    Vec3f randomVec = Vec3f(0.0f, 0.0f, 0.0f);

    if (fuzz > 0.0f) {
       randomVec = RayTracer::randomInUnitSphere();
    }

    return  (reflectionDir + randomVec * fuzz).normalize();
}

bool RayTracer::isInShadow(Vec3f N,HitRec & hitRec, Vec3f lightPosition) {
    Vec3f shadowOrigin = hitRec.p + N * Ray::rayEps; // Small offset to avoid self-hit
    Vec3f L = (lightPosition - hitRec.p).normalize();
    Ray shadowRay(shadowOrigin, L);
    shadowRay.tClip = (lightPosition - shadowOrigin).len(); // Stop at light

    HitRec shadowHitRec;

    searchClosestHit(shadowRay, shadowHitRec);

    if (shadowHitRec.anyHit && shadowHitRec.tHit < shadowRay.tClip) {
       return true;
    }

    return false;
}   
float RayTracer::computeShadowFactor(const HitRec& hitRec, const Light* light) {

    int hits = 0;
    int total = light->samples;
    Vec3f N = hitRec.n;

    for (int i = 0; i < total; i++) {
        Vec3f jitteredPos = light->position + randomInUnitSphere() * light->radius;
        Vec3f L = (jitteredPos - hitRec.p).normalize();

        Vec3f shadowOrigin = hitRec.p + N * Ray::rayEps;
        Ray shadowRay(shadowOrigin, L);
        shadowRay.tClip = (jitteredPos - shadowOrigin).len();

        HitRec shadowHitRec;
        searchClosestHit(shadowRay, shadowHitRec);

        if (shadowHitRec.anyHit && shadowHitRec.tHit < shadowRay.tClip) {
            hits++;

            if ((float)hits / total > 0.95f) {
                return 0.0f;
            }
        }
    }

    return 1.0f - (float)hits / total;
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
        inShadow = isInShadow(N, hitRec, light->position);
    #endif
   
   


    #ifdef SOFT_SHADOWS   
        // Soft shadow factor
        float shadowFactor = computeShadowFactor(hitRec, light);
        inShadow = (shadowFactor < 1.0f);
   
    #endif 

        shadow = inShadow;
    #ifdef SOFT_SHADOWS 
                if (inShadow && shadowFactor <= 0.01f) {
        #if defined(SHADOWS_BLACK)
                    return Vec3f(0.0f, 0.0f, 0.0f);
        #elif defined(SHADOWS_AMBIENT)
                    return ambient;
        #endif
                }
    #else
                if (inShadow) {
        #if defined(SHADOWS_BLACK)
                    return Vec3f(0.0f, 0.0f, 0.0f);
        #elif defined(SHADOWS_AMBIENT)
                    return ambient;
        #endif
        }
    #endif 

   
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
    
    #ifdef SOFT_SHADOWS
           localColor = ambient + (diffuse + specular) * shadowFactor;
    #else
           localColor = ambient + diffuse + specular ;
    #endif

       
        // Clamp color to [0, 1]
        localColor.x = min(max(0.0f, localColor.x), 1.0f);
        localColor.y = min(max(0.0f, localColor.y), 1.0f);
        localColor.z = min(max(0.0f, localColor.z), 1.0f);

        return localColor;
}


