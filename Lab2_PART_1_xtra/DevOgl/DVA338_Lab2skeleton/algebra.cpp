#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
#include "algebra.h"

Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

float DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}

void PrintVector(char const *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char const *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char const *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}

Vector SurfaceNormal(Vector a, Vector b, Vector c)
{
	Vector v1 = Subtract(b, a);
	Vector v2 = Subtract(c, a);
	Vector n = CrossProduct(v1, v2);
	n = Normalize(n);
	return n;
}


Matrix Translation(Vector t) {
	Matrix T = { 0 };
	T.e[0] = 1.0f; T.e[4] = 0.0f; T.e[8] = 0.0f; T.e[12] =t.x;

	T.e[1] = 0.0f; T.e[5] = 1.0f; T.e[9] = 0.0f; T.e[13] = t.y;

	T.e[2] = 0.0f; T.e[6] = 0.0f; T.e[10] = 1.0f; T.e[14] = t.z;

	T.e[3] = 0.0f; T.e[7] = 0.0f; T.e[11] = 0.0f; T.e[15] = 1.0f;
	return T;
}

Matrix Scaling(Vector s) {
	Matrix S = { 0 };
	S.e[0] = s.x; S.e[4] = 0.0f; S.e[8] = 0.0f; S.e[12] = 0.0f;

	S.e[1] = 0.0f; S.e[5] = s.y; S.e[9] = 0.0f; S.e[13] = 0.0f;

	S.e[2] = 0.0f; S.e[6] = 0.0f; S.e[10] = s.z; S.e[14] = 0.0f;

	S.e[3] = 0.0f; S.e[7] = 0.0f; S.e[11] = 0.0f; S.e[15] = 1.0f;
	return S;
}

Matrix RotationX(float angle) {

	angle = degToRad(angle);
	Matrix R = { 0 };

	R.e[0] = 1.0f; R.e[4] = 0.0f; R.e[8] = 0.0f; R.e[12] = 0.0f;

	R.e[1] = 0.0f; R.e[5] = cos(angle); R.e[9] = -sin(angle); R.e[13] = 0.0f;

	R.e[2] = 0.0f; R.e[6] = sin(angle); R.e[10] = cos(angle); R.e[14] = 0.0f;

	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	return R;
}
Matrix RotationY(float angle) {


	angle = degToRad(angle);
	Matrix R = { 0 };

	R.e[0] = cos(angle); R.e[4] = 0.0f; R.e[8] = sin(angle); R.e[12] = 0.0f;

	R.e[1] = 0.0f; R.e[5] = 1.0f; R.e[9] = 0.0f; R.e[13] = 0.0f;

	R.e[2] = -sin(angle); R.e[6] = 0.0f; R.e[10] = cos(angle); R.e[14] = 0.0f;

	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	return R;
}
Matrix RotationZ(float angle) {


	angle = degToRad(angle);

	Matrix R = { 0 };

	R.e[0] = cos(angle); R.e[4] = -sin(angle); R.e[8] = 0.0f; R.e[12] = 0.0f;

	R.e[1] = sin(angle); R.e[5] = cos(angle); R.e[9] = 0.0f; R.e[13] = 0.0f;

	R.e[2] = 0.0f; R.e[6] = 0.0f; R.e[10] = 1.0f; R.e[14] = 0.0f;

	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	return R;
}


float degToRad(float deg) {

	return deg *((float) M_PI / 180.0f);
}

Matrix RotationAxis(Vector axis, float angle) {
	angle = degToRad(angle);
	axis = Normalize(axis);
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	float c = cos(angle);
	float s = sin(angle);
	float oneMinusC = 1.0f - c;

	Matrix R = { 0 };

	R.e[0] = c + x * x * oneMinusC;
	R.e[4] = x * y * oneMinusC - z * s;
	R.e[8] = x * z * oneMinusC + y * s;
	R.e[12] = 0.0f;

	R.e[1] = y * x * oneMinusC + z * s;
	R.e[5] = c + y * y * oneMinusC;
	R.e[9] = y * z * oneMinusC - x * s;
	R.e[13] = 0.0f;

	R.e[2] = z * x * oneMinusC - y * s;
	R.e[6] = z * y * oneMinusC + x * s;
	R.e[10] = c + z * z * oneMinusC;
	R.e[14] = 0.0f;

	R.e[3] = 0.0f;
	R.e[7] = 0.0f;
	R.e[11] = 0.0f;
	R.e[15] = 1.0f;

	return R;
}


Matrix RotationAxis(Vector axisPoint, Vector axisDir, float angleDegrees) {
	// Normalize direction of the axis
	axisDir = Normalize(axisDir);

	float angleRad = degToRad(angleDegrees);

	// Step 1: Translate so axis passes through origin
	Matrix T_to_origin = Translation(ScalarVecMul(-1.0f, axisPoint));

	// Step 2: Align the axis with the Z-axis
	float theta = atan2f(axisDir.x, axisDir.z);  // Rotate around Y
	float phi = atan2f(axisDir.y, sqrtf(axisDir.x * axisDir.x + axisDir.z * axisDir.z)); // Rotate around X

	Matrix R_y_neg_theta = RotationY(-theta);
	Matrix R_x_neg_phi = RotationX(-phi);

	// Step 3: Rotate around Z-axis
	Matrix R_z = RotationZ(angleDegrees);

	// Step 4: Undo alignment
	Matrix R_x_phi = RotationX(phi);
	Matrix R_y_theta = RotationY(theta);

	// Step 5: Translate back to original position
	Matrix T_back = Translation(axisPoint);

	// Compose the transformations
	Matrix M = MatMatMul(T_back,
		MatMatMul(R_y_theta,
			MatMatMul(R_x_phi,
				MatMatMul(R_z,
					MatMatMul(R_x_neg_phi,
						MatMatMul(R_y_neg_theta,
							T_to_origin))))));

	return M;
}
