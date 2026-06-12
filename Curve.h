#pragma once
#include <array>
#include "Vector3.h"

struct Matrix4x4;

#pragma region Curve

/// <summary>
/// 曲線
/// </summary>
/// <param name="controlPoints">制御点</param>
class Curve {
public:
	Curve() = default;
	Curve(const std::array<Vector3, 3>& controlPoints, unsigned int color = 0xFFFFFFFF)
		: controlPoints_(controlPoints), color_(color) {}

	void Update();
	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	void DrawBezier(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
	void DrawControlPoints(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	const std::array<Vector3, 3>& GetControlPoints() const { return controlPoints_; }

	//void SetControlPoints(const std::array<Vector3, 3>& points) { controlPoints_ = points; }
	void SetColor(unsigned int p) { color_ = p; }

private:
	std::array<Vector3, 3> controlPoints_ = {
	    Vector3{-0.8f, 0.58f, 1.0f },
	    Vector3{1.76f, 1.0f,  -0.3f},
	    Vector3{0.94f, -0.7f, 2.3f },
	};
	unsigned int color_;
	bool isDrawControlPoints_ = true;
};

#pragma endregion