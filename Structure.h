#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Sphere

struct SphereDesc {
	Vector3 center{0, 0, 0};
	float radius{1.0f};
	float moveSpeed{0.0f};
	unsigned int color{0xFFFFFFFF};
};

/// <summary>
/// 球
/// </summary>
/// <param name="Vector3  center_">位置</param>
/// <param name="float  radius_">半径</param>
/// <param name="unsigned int  color_">色</param>
class Sphere {
public:
	Sphere(const SphereDesc& desc) : center_(desc.center), radius_(desc.radius), moveSpeed_(desc.moveSpeed), color_(desc.color) {}

	void UpdateToKeyMove(const char* keys);

	const Vector3 GetCenter() const { return center_; }
	const float GetRadius() const { return radius_; }

	void SetCenter(Vector3 p) { center_ = p; }
	void SetRadius(float p) { radius_ = p; }
	void SetColor(unsigned int p) { color_ = p; }

	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

private:
	Vector3 center_; //!< 中心点
	float radius_;   //!< 半径
	float moveSpeed_;
	unsigned int color_;
};

#pragma endregion

#pragma region Plane

struct PlaneDesc {
	Vector3 normal{0.0f, 1.0f, 0.0f};
	float distance{1.5f};
};

/// <summary>
/// 平面
/// </summary>
/// <param name="Vector3  normal">法線</param>
/// <param name="float  distance">距離</param>
class Plane {
public:
	Plane(const PlaneDesc& desc);
	Vector3 Perpendicular(const Vector3& vector) const;

	const Vector3 GetNormal() const { return normal_; }
	const float GetDistance() const { return distance_; }

	void SetNormal(const Vector3& p);
	void SetDistance(float p) { distance_ = p; }

	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

private:
	Vector3 normal_; //!< 法線
	float distance_; //!< 距離
};

#pragma endregion

#pragma region Segment

struct SegmentDesc {
	Vector3 origin{-0.45f, 0.41f, 0.0f}; //!< 始点
	Vector3 diff{1.0f, 0.58f, 0.0f};   //!< 終点への差分ベクトル
	unsigned int color{0xFFFFFFFF};
};

/// <summary>
/// 線
/// </summary>
/// <param name="Vector3  origin">始点</param>
/// <param name="Vector3  diff">終点への差分ベクトル</param>
class Segment {
public:
	Segment(const SegmentDesc& desc) : origin_(desc.origin), diff_(desc.diff), color_(desc.color) {}

	Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

	const Vector3 GetOrigin() const { return origin_; }
	const Vector3 GetDiff() const { return diff_; }

	void SetOrigin(Vector3 p) { origin_ = p; }
	void SetDiff(Vector3 p) { diff_ = p; }
	void SetColor(unsigned int p) { color_ = p; }

	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

private:
	Vector3 origin_; //!< 始点
	Vector3 diff_;   //!< 終点への差分ベクトル
	unsigned int color_;
};

#pragma endregion

// グリッド描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

// デバッグ用Printf関数
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);
