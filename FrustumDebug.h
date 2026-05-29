#pragma once

struct Matrix4x4;
struct Vector3;
class Sphere;

void DrawFrustum(const Matrix4x4& cameraViewProjection, const Matrix4x4& drawViewProjection, const Matrix4x4& viewportMatrix);

struct FrustumPlane {
	Vector3 normal;
	float distance;
};
struct Frustum {
	FrustumPlane planes[6];
};

Frustum CreateFrustum(const Matrix4x4& vp);
bool IsInsideFrustum(const Sphere& sphere, const Frustum& frustum);