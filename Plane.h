#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Plane

/// <summary>
/// 平面
/// </summary>
/// <param name="Vector3  normal">法線</param>
/// <param name="float  distance">距離</param>
class Plane {
public:

	Plane() = default;
    Plane(const Vector3& normal, float distance)
    : distance_(distance) {
    SetNormal(normal);
}

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	Vector3 Perpendicular(const Vector3& vector) const;

	const Vector3 GetNormal() const { return normal_; }
	const float GetDistance() const { return distance_; }

	void SetNormal(const Vector3& p);
	void SetDistance(float p) { distance_ = p; }

private:
	Vector3 normal_; //!< 法線
	float distance_; //!< 距離
};

#pragma endregion
