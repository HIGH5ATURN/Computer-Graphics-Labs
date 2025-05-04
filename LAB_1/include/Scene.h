#ifndef _SCENE_H_
#define _SCENE_H_
#include <vector>
using namespace std;

#include "Sphere.h"
#include "Light.h"
#include "Plane.h"
#include "Triangle.h"
class Scene {
private:

public:
	vector<Object*> objects;		
	vector<Light*> lights;
	Scene(void) {}
	~Scene() {
		clear();
	}
	void addSphere(const Vec3f & c, const float r,const Material* mat) {
		//Store sphere as an Object pointer
        Sphere* s = new Sphere(c, r, mat);
		objects.push_back(s);
	}

    void addLight(const Vec3f& pos, const Vec3f& ambient, const Vec3f& diffuse, const Vec3f& specular, float rad, int samp) {
       // Corrected to use a pointer to Light  
       Light* l = new Light(pos, ambient, diffuse,specular,rad,samp);
	   lights.push_back(l);
    }

	void addPlane(const Vec3f& pt, const Vec3f& n, const Material* mat) {
		Plane* plane = new Plane(pt, n, mat);
		objects.push_back(plane);
	}

	void addTriangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Material* mat,bool flip=false) {
		Triangle* triangle = new Triangle(a, b, c, mat,flip);
		objects.push_back(triangle);
	}


	void clear()
	{
		// clear objects from memory
		for(vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it)
			delete *it;
		// Then reset the container
		objects.clear();
	}
	
	int save(char * fileName) {
		// Save your scene to a file somehow ...
		return 0;
	}
	void load(char * fileName) {
		// To avoid memory leaks, we run clear, which deletes any old objects first.
		this->clear();
		// load scene from file here ...
	}

};

#endif