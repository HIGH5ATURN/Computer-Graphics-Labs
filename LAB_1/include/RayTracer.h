#ifndef _RayTracer_H_
#define _RayTracer_H_

#include "Scene.h"
#include "Image.h"
#include "Ray.h"
#include "Sphere.h"

class RayTracer {
private: 
	Scene * scene;
	Image * image;
    Vec3f bgColor;
    Vec3f bgColor2;
public:
    RayTracer(const int w, const int h) {
        this->scene = new Scene();
        this->image = new Image(w, h);
       /* this->bgColor = Vec3f(0.69f, 0.69f, 0.69f);*/

        this->bgColor = Vec3f(0.0627f, 0.0706f, 0.0941f);
        this->bgColor2= Vec3f(0.69f, 0.69f, 0.69f);
    }
    ~RayTracer() {
        delete scene;
        delete image;
    }
    void searchClosestHit(const Ray & ray, HitRec & hitRec);
	Vec3f getEyeRayDirection(int x, int y);
    void fireRays();
    Vec3f traceRay(const Ray& ray, int depth);
    //--calculating the colors
    Vec3f computeLightColor(const Ray& ray, HitRec& hitRec, const Light* light,bool &shadow);
    
    Vec3f calculateReflection(const Ray& ray, HitRec& hitRec, int depth);

    Vec3f randomlyPerturbNormals(Vec3f reflectionDir, const Material* mat);

    bool isInShadow(Vec3f N, HitRec& hitRec, Vec3f lightPosition);

    float computeShadowFactor(const HitRec& hitRec, const Light* light);

    Vec3f calculateRefraction(const Ray& ray, HitRec& hitRec, int depth);
    void toPPM(const char* path) {
        this->image->toPPM(path);
    };
    void toBMP(const char* path) {
        this->image->toBMP(path);
    };
    void addSphere(const Vec3f & c, const float r, const Material* mat) {
        if (r >= 0) {
            this->scene->addSphere(c, r, mat);
        }
       
    };

    void addLight(const Vec3f& pos, const Vec3f& ambient,const Vec3f& diffuse, const Vec3f& specular, float rad, int samp) {

        this->scene->addLight(pos, ambient, diffuse, specular, rad, samp);
    };

    void addPlane(const Vec3f& pt, const Vec3f& n, const Material* mat) {
        this->scene->addPlane(pt,  n,  mat);
    }

    void addTriangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Material* mat,bool flip=false) {
       
        this->scene->addTriangle(a, b, c, mat,flip);
    }

     Vec3f randomInUnitSphere() {
        while (true) {
            Vec3f p(
                ((float)rand() / RAND_MAX) * 2.0f - 1.0f,
                ((float)rand() / RAND_MAX) * 2.0f - 1.0f,
                ((float)rand() / RAND_MAX) * 2.0f - 1.0f
            );
            if (p.lenSq() < 1.0f) return p;
        }
    }
};

#endif