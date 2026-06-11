#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Sphere

/// <summary>
/// 球
/// </summary>
/// <param name="Vector3  center_">位置</param>
/// <param name="float  radius_">半径</param>
/// <param name="unsigned int  color_">色</param>
class Sphere {
public:
	Sphere() = default;
	Sphere(const Vector3& center, float radius, unsigned int color = 0xFFFFFFFF)
		: center_(center), radius_(radius), color_(color) {}

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	const Vector3 GetCenter() const { return center_; }
	const float GetRadius() const { return radius_; }

	void SetCenter(Vector3 p) { center_ = p; }
	void SetRadius(float p) { radius_ = p; }
	void SetColor(unsigned int p) { color_ = p; }

	void SetHit(bool hit) { isHit_ = hit; }
	bool IsHit() const { return isHit_; }

private:
	Vector3 center_; //!< 中心点
	float radius_;   //!< 半径
	bool isHit_ = false;
	unsigned int color_;
};

#pragma endregion
