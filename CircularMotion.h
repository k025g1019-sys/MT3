#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region CircularMotion

/// <summary>
/// 等速円運動
/// </summary>
class CircularMotion {
public:
	CircularMotion();

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

private:
	// 円
	Vector3 center_;        // 円の中心
	float radius_;          // 円の半径[m]
	float angularVelocity_; // 角速度[rad/s]
	float angle_;           // 現在の角度[rad]

	// 球
	Vector3 position_;   // 球の位置
	float ballRadius_;   // 球の半径
	unsigned int color_; // 球の色

	// 60fpsなのでdeltaTimeは1/60秒
	float deltaTime_ = 1.0f / 60.0f;

	// Startボタンが押されたら動き始める
	bool isStart_ = false;
};

#pragma endregion
