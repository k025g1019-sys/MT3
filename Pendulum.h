#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Pendulum

/// <summary>
/// 振り子
/// </summary>
class Pendulum {
public:
	Pendulum();

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

private:
	// 振り子
	Vector3 anchor_;            // アンカーポイント。固定された端の位置
	float length_;             // 紐の長さ
	float angle_;              // 現在の角度
	float angularVelocity_;    // 角速度ω
	float angularAcceleration_; // 角加速度

	// 先端の球
	Vector3 position_;   // 振り子の先端の位置
	float ballRadius_;   // 球の半径
	unsigned int color_; // 球の色

	// 60fpsなのでdeltaTimeは1/60秒
	float deltaTime_ = 1.0f / 60.0f;

	// Startボタンが押されたら動き始める
	bool isStart_ = false;
};

#pragma endregion
