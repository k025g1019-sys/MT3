#pragma once
#include "Vector3.h"
#include <vector>

struct Matrix4x4;

#pragma region Sphere

struct SphereDesc {
	Vector3 center{0, 0, 0};
	float radius{1.0f};
	float moveSpeed{0.0f};
	unsigned int color{0xFFFFFFFF};
};

// 単体責務
class Sphere {
public:
	Sphere(const SphereDesc& desc) : center_(desc.center), radius_(desc.radius), moveSpeed_(desc.moveSpeed), color_(desc.color) {}

	void UpdateToKeyMove(const char* keys);
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	const Vector3 GetCenter() const { return center_; }
	const float GetRadius() const { return radius_; }

	void SetCenter(Vector3 p) { center_ = p; }
	void SetRadius(float p) { radius_ = p; }
	void SetColor(unsigned int p) { color_ = p; }

private:
	Vector3 center_; // 中心点
	float radius_;   // 半径
	float moveSpeed_;
	unsigned int color_;
};

#pragma endregion

#pragma region Plane

struct PlaneDesc {
	Vector3 normal{0.0f, 1.0f, 0.0f};
	float distance{1.5f};
};

class Plane {
public:
	Plane(const PlaneDesc& desc) : normal(desc.normal), distance(desc.distance) {}
	Vector3 Perpendicular(const Vector3& vector) const;
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	const Vector3 GetNormal() const { return normal; }
	const float GetDistance() const { return distance; }

	void SetNormal(Vector3& p) { normal = p; }
	void SetDistance(float& p) { distance = p; }

private:
	Vector3 normal; //!< 法線
	float distance; //!< 距離
};

#pragma endregion

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
