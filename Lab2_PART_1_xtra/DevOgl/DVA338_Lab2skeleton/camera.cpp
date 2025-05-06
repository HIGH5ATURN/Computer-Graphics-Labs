#include "camera.h"
#include "math.h"
Matrix PerspectiveProjectionMatrix(Camera cam, int screenWidth, int screenHeight)
{

	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform.
	Matrix P = { 0.0f };
	//P.e[0] = 1.299038f; P.e[4] = 0.000000f; P.e[ 8] =  0.000000f; P.e[12] =  0.0f;
	//P.e[1] = 0.000000f; P.e[5] = 1.732051f; P.e[ 9] =  0.000000f; P.e[13] =  0.0f;
	//P.e[2] = 0.000000f; P.e[6] = 0.000000f; P.e[10] = -1.000200f; P.e[14] = -2.000200f;
	//P.e[3] = 0.000000f; P.e[7] = 0.000000f; P.e[11] = -1.000000f; P.e[15] =  0.0f;

	float aspect = (float)screenWidth / (float)screenHeight;

	float fovRad = degToRad(cam.fov);

	float cotanFov = 1.0f / tan(fovRad / 2.0f);

	float far = cam.farPlane;

	float near = cam.nearPlane;

	P.e[0] = cotanFov / aspect;

	P.e[5] = cotanFov;
	
	P.e[10] = (far + near)/(near-far);
	
	P.e[11] = -1.0f;
	
	P.e[14] = (2.0f * far * near) / (near - far);

	P.e[1] = P.e[2] = P.e[3] = 0.0f;
	P.e[4] = P.e[6] = P.e[7] = 0.0f;
	P.e[8] = P.e[9] = 0.0f;
	P.e[12] = P.e[13] = 0.0f;
	P.e[15] = 0.0f;

	return P;
}

Matrix OrthogonalProjectionMatrix(float left, float right, float top, float bottom, float near, float far) {
	Matrix P = { 0.0f };

	P.e[0] = 2.0f / (right - left);
	P.e[5] = 2.0f / (top - bottom);
	P.e[10] = 2.0f / (near - far);
	P.e[12] = -(right + left) / (right - left);
	P.e[13] = -(top + bottom) / (top - bottom);
	P.e[14] = -(far + near) / (far - near);
	P.e[15] = 1;

	return P;
}

//Interface function for view matrix
Matrix ViewMatrix(Camera cam)
{
	// Compute camera basis vectors
	Vector z = Normalize(ScalarVecMul(-1.00f, cam.viewDir));
	Vector x = Normalize(CrossProduct(cam.up, z));   
	Vector y = CrossProduct(z, x);                      
	Vector c = cam.position;

	Matrix V;

	// View matrix in column-major order for OpenGL
	//
	// |  x.x     y.x     z.x     -dot(x, c)  |
	// |  x.y     y.y     z.y     -dot(y, c)  |
	// |  x.z     y.z     z.z     -dot(z, c)  |
	// |  0.0     0.0     0.0        1.0      |

	V.e[0] = x.x;   V.e[4] = y.x;   V.e[8] = z.x;   V.e[12] = -DotProduct(x, c);
	V.e[1] = x.y;   V.e[5] = y.y;   V.e[9] = z.y;   V.e[13] = -DotProduct(y, c);
	V.e[2] = x.z;   V.e[6] = y.z;   V.e[10] = z.z;   V.e[14] = -DotProduct(z, c);
	V.e[3] = 0.0f;  V.e[7] = 0.0f;  V.e[11] = 0.0f;  V.e[15] = 1.0f;

	return V;
}


Vector Right(Camera cam) {
	return Normalize(CrossProduct(cam.viewDir, cam.up));
}

void MoveForward(Camera* cam, float amount) {
	Vector delta = ScalarVecMul(amount, cam->viewDir);
	cam->position = Add(cam->position, delta);
}

void MoveRight(Camera* cam, float amount) {
	Vector r = Right(*cam);
	Vector delta = ScalarVecMul(amount, r);
	cam->position = Add(cam->position, delta);
}

void MoveUp(Camera* cam, float amount) {
	Vector delta = ScalarVecMul(amount, cam->up);
	cam->position = Add(cam->position, delta);
}

void MoveLeft(Camera* cam, float amount) {
	// Calculate the right direction and move in that direction
	Vector r = Right(*cam);
	Vector delta = ScalarVecMul(-amount, r);  // Move left (negative right direction)
	cam->position = Add(cam->position, delta);
}

void Pitch(Camera* cam, float angleDegrees) {
	// Rotate around the right vector
	Vector r = Right(*cam);  // Get the right vector
	Matrix R = RotationAxis(cam->position,r, angleDegrees);  // Create the rotation matrix

	// Apply the rotation to viewDir and up
	cam->viewDir = Normalize(Homogenize(MatVecMul(R, cam->viewDir)));
	cam->up = Normalize(Homogenize(MatVecMul(R, cam->up)));
}

void Yaw(Camera* cam, float angleDegrees) {
	// Rotate around the up vector
	Matrix R = RotationAxis(cam->position,cam->up, angleDegrees);  // Rotation matrix around up vector

	// Apply the rotation to viewDir
	cam->viewDir = Normalize(Homogenize(MatVecMul(R, cam->viewDir)));
}

void Roll(Camera* cam, float angleDegrees) {
	// Rotate around the view direction
	Matrix R = RotationAxis(cam->position,cam->viewDir, angleDegrees);  // Rotation matrix around viewDir

	// Apply the rotation to up
	cam->up = Normalize(Homogenize(MatVecMul(R, cam->up)));
}

Matrix LookAtMatrix(Camera cam, float pitchDeg, float yawDeg, float rollDeg)
{
	// Create a rotation matrix from yaw, pitch, roll (in that order)
	Matrix yawMatrix = RotationAxis(cam.up, yawDeg);
	Vector right = Normalize(CrossProduct(cam.viewDir, cam.up));
	Matrix pitchMatrix = RotationAxis(right, pitchDeg);
	Matrix rollMatrix = RotationAxis(cam.viewDir, rollDeg);

	// Compose the full rotation matrix: R = roll * pitch * yaw
	Matrix R1 = MatMatMul(pitchMatrix, yawMatrix);
	Matrix R = MatMatMul(rollMatrix, R1);

	// Apply the rotation to the original direction vectors
	Vector viewDir = Normalize(Homogenize(MatVecMul(R, cam.viewDir)));
	Vector up = Normalize(Homogenize(MatVecMul(R, cam.up)));

	// Now compute the View matrix from the rotated vectors and position
	Vector z = Normalize(ScalarVecMul(-1.0f, viewDir));
	Vector x = Normalize(CrossProduct(up, z));
	Vector y = CrossProduct(z, x);
	Vector c = cam.position;

	Matrix V;

	V.e[0] = x.x;   V.e[4] = y.x;   V.e[8] = z.x;   V.e[12] = -DotProduct(x, c);
	V.e[1] = x.y;   V.e[5] = y.y;   V.e[9] = z.y;   V.e[13] = -DotProduct(y, c);
	V.e[2] = x.z;   V.e[6] = y.z;   V.e[10] = z.z;  V.e[14] = -DotProduct(z, c);
	V.e[3] = 0.0f;  V.e[7] = 0.0f;  V.e[11] = 0.0f; V.e[15] = 1.0f;

	return V;
}
