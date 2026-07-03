#pragma once
#include "Vector3.h"

struct Matrix4x4;
class Plane;

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
	Sphere(const Vector3& center, float radius, unsigned int color = 0xFFFFFFFF, bool enableGravity = false, float mass = 1.0f, float restitution = 0.8f)
	    : center_(center), radius_(radius), color_(color), enableGravity_(enableGravity), prevCenter_(center), mass_(mass), restitution_(restitution), initialCenter_(center) {
		if (enableGravity_) {
			acceleration_ = {0.0f, -9.8f, 0.0f}; // 重力加速度
		}
	}

	void Update();
	// 平面との衝突後、めり込みを解消して速度を反射方向に変更する
	void BounceOffPlane(const Plane& plane);
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	const Vector3 GetCenter() const { return center_; }
	const Vector3 GetPrevCenter() const { return prevCenter_; }
	const float GetRadius() const { return radius_; }
	bool IsGravityEnabled() const { return enableGravity_; }

	void SetCenter(Vector3 p) { center_ = p; }
	void SetRadius(float p) { radius_ = p; }
	void SetColor(unsigned int p) { color_ = p; }
	void SetVelocity(const Vector3& v) { velocity_ = v; }

	void SetHit(bool hit) { isHit_ = hit; }
	bool IsHit() const { return isHit_; }

private:
	Vector3 center_{}; //!< 中心点
	float radius_ = 0.0f;   //!< 半径
	bool isHit_ = false;
	unsigned int color_ = 0xFFFFFFFF;

	// 物理挙動(重力)
	bool enableGravity_ = false; // 重力を有効にするか
	Vector3 prevCenter_{};       // 1フレーム前の位置(スイープ判定用)
	Vector3 velocity_{};         // 速度
	Vector3 acceleration_{};     // 加速度
	float mass_ = 1.0f;          // 質量
	float restitution_ = 0.8f;   // 反発係数e

	// Reset用の初期値
	Vector3 initialCenter_{};
	Vector3 initialVelocity_{};

	// 60fpsなのでdeltaTimeは1/60秒
	float deltaTime_ = 1.0f / 60.0f;

	// Startボタンが押されたら動き始める
	bool isStart_ = false;
};

#pragma endregion
