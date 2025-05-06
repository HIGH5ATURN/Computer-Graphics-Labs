#ifndef _CAMERA_H
#define _CAMERA_H
#include "algebra.h"

typedef struct Camera {
    Vector position; // Camera position
    Vector viewDir;  // Normalized view direction
    Vector up;       // Normalized up vector
    double fov;
    double nearPlane;
    double farPlane;
    bool isOrtho;
} Camera;

Matrix PerspectiveProjectionMatrix(Camera cam, int screenWidth, int screenHeight);
Matrix ViewMatrix(Camera cam);
Matrix OrthogonalProjectionMatrix(float left, float right, float top, float bottom, float near, float far);

Vector Right(Camera cam);
void MoveForward(Camera* cam, float amount);
void MoveRight(Camera* cam, float amount);
void MoveUp(Camera* cam, float amount);
void MoveLeft(Camera* cam, float amount);
void Pitch(Camera* cam, float angleDegrees);
void Yaw(Camera* cam, float angleDegrees);
void Roll(Camera* cam, float angleDegrees);

#endif
