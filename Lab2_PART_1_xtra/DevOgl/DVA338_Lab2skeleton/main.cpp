//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
	//If you follow the setup instructions provided for Visual Studio
	//The include path specified already includes "GL", so we can ignore it
	#include <glew.h>
	#include <freeglut.h>
#elif defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
	#include <GL/glew.h>
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/freeglut.h>
#endif
#include "camera.h"	//algebra is included in camera.h
#include "shaders.h"
#include "mesh.h"
#define M_PI       3.14159265358979323846
Camera cam = {
	{0, 0, 20},      // position
	{0, 0, -1},      // viewDir (normalized)
	{0, 1, 0},       // up (normalized)
	60.0,            // fov
	1.0,             // nearPlane
	10000.0,         // farPlane
	false            // isOrtho
};

int screen_width = 1024;
int screen_height = 768;

Mesh *meshList = NULL; // Global pointer to linked list of triangle meshes

GLuint shprg; // Shader program id
int windowHandle;

// Global transform matrices
// V is the view transform
// P is the projection transform
// PV = P * V is the combined view-projection transform
Matrix V, P, PV;
void printShaderErrorInfo(GLuint shaderProgram)
{
	GLint maxLength = 0;
	glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	GLchar * errorLog = (GLchar*)calloc(maxLength, sizeof(GLchar));
	glGetShaderInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

	// Provide the infolog in whatever manor you deem best.
	printf("%s\n", errorLog);
	printf("\n");
	free(errorLog);
}
void prepareShaderProgram(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;

	shprg = glCreateProgram();
	glEnable(GL_DEPTH_TEST);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");
	if(success == GL_FALSE)
	{
		printShaderErrorInfo(vs);
		glDeleteShader(vs); // Don't leak the shader.
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");
	if(success == GL_FALSE)
	{
		printShaderErrorInfo(fs);
		glDeleteShader(fs); // Don't leak the shader.
	}

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void prepareMesh(Mesh *mesh) {
	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols  = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);
	
	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 *sizeof(float)));

	glBindVertexArray(0);

}

void renderMesh(Mesh *mesh) {
	
	// Assignment TODO: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below

	// Local transformations
	Matrix S = Scaling({ mesh->scale.x, mesh->scale.y, mesh->scale.z });
	Matrix R = RotationZ(mesh->rotation.z);
	R = MatMatMul(RotationY(mesh->rotation.y),R);
	R = MatMatMul(RotationX(mesh->rotation.x),R);
	Matrix T = Translation({ mesh->translation.x, mesh->translation.y, mesh->translation.z });
	Matrix W = MatMatMul(T, MatMatMul(R, S));
	Matrix M = MatMatMul(V, W);
	Matrix PVM = MatMatMul(P, M);

	// Pass the viewing transform to the shader
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, PVM.e);


	// Select current resources 
	glBindVertexArray(mesh->vao);
	
	// To accomplish wireframe rendering (can be removed to get filled triangles)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL); 
	glBindVertexArray(0);
}



void display(void) {
	Mesh *mesh;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*
	Assignment TODO: The implementation of the View- and ProjectionMatrix functions in camera.cpp 
	returns hardcoded matrices. Your assignment is to fix them.
	*/
	V = ViewMatrix(cam);
	
	if (cam.isOrtho) {
		P = OrthogonalProjectionMatrix(-20, 20, 10, -10, 1, 1000);
	}
	else {
		P = PerspectiveProjectionMatrix(cam, screen_width, screen_height);
	}
	
	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);

	// Select the shader program to be used during rendering 
	glUseProgram(shprg);

	// Render all meshes in the scene
	mesh = meshList;
		
	while (mesh != NULL) {
		renderMesh(mesh);
		mesh = mesh->next;
	}

	glFlush();
}

void changeSize(int w, int h) {
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);

}
void cleanUp(void);
void keypress(unsigned char key, int x, int y) {
    float step = 0.2f;
    float angleStep = 5.0f; // degrees

    switch (key) {
    // Movement
    case 'w': MoveForward(&cam, step); break;
    case 's': MoveForward(&cam, -step); break;
    case 'a': MoveRight(&cam, -step); break;
    case 'd': MoveRight(&cam, step); break;
    case 'q': MoveUp(&cam, step); break;
    case 'e': MoveUp(&cam, -step); break;

    // Rotation
    case 'i': Pitch(&cam, -angleStep); break;
    case 'k': Pitch(&cam, angleStep); break;
    case 'j': Yaw(&cam, -angleStep); break;
    case 'l': Yaw(&cam, angleStep); break;
    case 'u': Roll(&cam, -angleStep); break;
    case 'o': Roll(&cam, angleStep); break;

    // Toggle projection
    case 'p': cam.isOrtho = !cam.isOrtho; break;
    }

    glutPostRedisplay();
}


void init(void) {
	// Compile and link the given shader program (vertex shader and fragment shader)
	prepareShaderProgram(vs_n2c_src, fs_ci_src); 

	// Setup OpenGL buffers for rendering of the meshes
	Mesh * mesh = meshList;
	while (mesh != NULL) {
		prepareMesh(mesh);
		mesh = mesh->next;
	}	
}

void cleanUp(void) {	
	printf("Running cleanUp function... ");
	// Free openGL resources
	// ...
	glutDestroyWindow(windowHandle);

	// Free meshes
	// ...
	
	printf("Done!\n\n");
}

// Include data for some triangle meshes (hard coded in struct variables)
#include "./models/mesh_bunny.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"


int main(int argc, char **argv) {
	//cam.rotation.x = degToRad(cam.rotation.x);
	//cam.rotation.y = degToRad(cam.rotation.y);
	//cam.rotation.z = degToRad(cam.rotation.z);
	// Setup freeGLUT
	unsigned int _glut_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;

	#ifdef __APPLE__
	_glut_mode |= GLUT_3_2_CORE_PROFILE;
	#endif
	glutInitDisplayMode(_glut_mode);
	glutInit(&argc, argv);
	glutInitWindowSize(screen_width, screen_height);
	windowHandle = glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	//This is a freeglut define value. In MacOS GLUT, these functions do not exist.
	#ifdef GLUT_CORE_PROFILE 
  	glutInitContextVersion(4, 1);
  	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	#endif
	
	glewExperimental = GL_TRUE;
	// Uses GLEW as OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit(); 
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list	
	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	 insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	meshList->translation = { 0, 1.5, -8 };
	meshList->rotation = { 0, -90, 0 };
	meshList->scale = { 1, 1, 1 };

	insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	meshList->translation = { -6.8, 0.8, 5 };
	meshList->rotation = { 0, 0, 0 };
	meshList->scale = { 0.1, 0.1, 0.1 };

	
	//insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);	
	//insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	//insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 1.0);
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);

	insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);
	meshList->translation = { 0, -7, 0 };
	meshList->rotation = { -90, 0, 180 };
	meshList->scale = { 1, 1, 1 };

	init();
	glutMainLoop();

	cleanUp();	
	return 0;
}
