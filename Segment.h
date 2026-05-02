#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Segment

/// <summary>
/// 線
/// </summary>
/// <param name="Vector3  origin">始点</param>
/// <param name="Vector3  diff">終点への差分ベクトル</param>
class Segment{
public:
	Segment() = default;
    Segment(const Vector3& origin, const Vector3& diff, unsigned int color = 0xFFFFFFFF)
        : origin_(origin), diff_(diff), color_(color) {}

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

	const Vector3 GetOrigin() const { return origin_; }
	const Vector3 GetDiff() const { return diff_; }

	void SetOrigin(Vector3 p) { origin_ = p; }
	void SetDiff(Vector3 p) { diff_ = p; }
	void SetColor(unsigned int p) { color_ = p; }

private:
	Vector3 origin_; //!< 始点
	Vector3 diff_;   //!< 終点への差分ベクトル
	unsigned int color_;
};

#pragma endregion
