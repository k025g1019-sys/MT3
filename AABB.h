#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region AABB

/// <summary>
/// AABB
/// </summary>
/// <param name="Vector3  position">位置</param>
/// <param name="Vector3  min">最小点</param>
/// <param name="Vector3  max">最大点</param>
class AABB {
public:
	AABB() = default;
    AABB(const Vector3& min, const Vector3& max, unsigned int color = 0xFFFFFFFF)
        : min_(min), max_(max), color_(color) {}

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	void Normalize();

	const Vector3& GetMin() const { return min_; }
	const Vector3& GetMax() const { return max_; }

	const Vector3& GetPosition() const { return position_; }

	void SetPosition(const Vector3& position) { position_ = position; }

	AABB GetWorldAABB() const {
    return AABB(
        min_ + position_,
        max_ + position_,
        color_
    );
	}

	void SetMin(Vector3 p) {
		min_ = p;
		Normalize();
	}
	void SetMax(Vector3 p) {
		max_ = p;
		Normalize();
	}
	void SetMinMax(const Vector3& min, const Vector3& max) {
		min_ = min;
		max_ = max;
		Normalize();
	}
	void SetColor(unsigned int p) { color_ = p; }

private:
	Vector3 position_{0.0f, 0.0f, 0.0f};
	Vector3 min_{-0.5f, -0.5f, -0.5f}; //!< 最小点
	Vector3 max_{0.0f, 0.0f, 0.0f};    //!< 最大点
	unsigned int color_{0xFFFFFFFF};
};

#pragma endregion
