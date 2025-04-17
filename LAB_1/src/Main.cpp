#include "RayTracer.h"
#include "Vec3.h" 
#include "raytrace_features.h"
#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
RayTracer* initTracer(int width, int height)
{
	RayTracer* rayTracer = new RayTracer(width, height);

	//Green
	rayTracer->addSphere(Vec3f(2.25f, -0.05f, -10.0f), 0.95f, new Material(
		Vec3f(0.0f, 0.1f, 0.0f),   // ambient
		Vec3f(0.1f, 0.6f, 0.1f),   // diffuse
		Vec3f(0.6f, 0.6f, 0.6f),   // specular
		5000.0f,//shineness
		0.5f//reflectivity
	));

	//RED
	rayTracer->addSphere(Vec3f(0.0f, -0.05f, -10.0f), 1.0f, new Material(
		Vec3f(0.1f, 0.0f, 0.0f),   // ambient
		Vec3f(0.6f, 0.1f, 0.1f),   // diffuse
		Vec3f(0.06f, 0.06f, 0.06f),   // specular
		4000.0f,                  // shininess
		0.5f

	));


	//yellow
	rayTracer->addSphere(Vec3f(-20.0f, 20.00f, -7.0f), 7.00f, new Material(
		Vec3f(0.6f, 0.6f, 0.0f),                        
		Vec3f(0.9f, 0.9f, 0.0f),                         
		Vec3f(0.5f, 0.5f, 0.5f),
		5000.0f,
		0.5f
	));

	#ifdef TRIANGLES



	/*Triangles*/
		//Red Triangle
	rayTracer->addTriangle(
		Vec3f(-0.8f, 1.0f, -11.0f), 
		Vec3f(-2.5f, 3.4f, -11.0f), 
		Vec3f(-0.8f, 3.4f, -11.0f),  
		new Material(
		Vec3f(0.2f, 0.0f, 0.0f),
		Vec3f(0.5f, 0.0f, 0.0f),
		Vec3f(0.2f, 0.2f, 0.2f),   
		4000.0f ,
		0.5f)               
	);

	//Green Triangle
	rayTracer->addTriangle(
		Vec3f(-0.8f, 1.0f, -11.0f), 
		Vec3f(0.9f, 3.4f, -11.0f), 
		Vec3f(-0.8f, 3.4f, -11.0f), 
		new Material(Vec3f(0.0f, 0.2f, 0.0f),
		Vec3f(0.0f, 0.5f, 0.0f),
		Vec3f(0.2f, 0.2f, 0.2f),    
		4000.0f ,
			0.5f)                
	);



	#endif 




	/*Ground*/



	//// Plane - mirror-like floor (unchanged)
	//rayTracer->addPlane(
	//	Vec3f(0.0f, -1.0f, 0.0f),   // point
	//	Vec3f(0.0f, 1.0f, 0.0f),    // normal
	//	new Material(
	//		Vec3f(0.0f, 0.0f, 0.3f),
	//		Vec3f(0.0f, 0.0f, 0.6f),
	//		Vec3f(0.5f, 0.5f, 0.5f),
	//		10000.0f,
	//		0.98f
	//	)
	//);


	// Ground Sphere - subtle blue, matte surface
	const Material* blueGround = new Material(
		Vec3f(0.02f, 0.02f, 0.05f),  // ambient
		Vec3f(0.1f, 0.1f, 0.6f),     // diffuse
		Vec3f(0.0f, 0.0f, 0.0f),     // specular (matte surface)
		10.0f,                       // low shininess
		0.75f                         // non-reflective
	);





	// Big sphere to simulate the ground
	rayTracer->addSphere(Vec3f(0.0f, -1001.0f, -10.0f), 1000.0f, blueGround);



	rayTracer->addLight(Vec3f(-10.0f,5.0f, 10.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f));

	return rayTracer;
}

void runTracer(RayTracer* tracer)
{
	tracer->fireRays();
	tracer->toPPM("DVA338_Raytracer_output.ppm");
	tracer->toBMP("DVA338_Raytracer_output.bmp");
}

void destroyTracer(RayTracer* tracer)
{
	delete tracer;
}

int main(int argc, char **argv) {
	RayTracer* tracer = initTracer(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	runTracer(tracer);
	destroyTracer(tracer);
	return 0;
}
