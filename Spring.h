#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Spring

// ボール
struct Ball {
	Vector3 position;     // ボールの位置
	Vector3 velocity;     // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass;           // ボールの質量
	float radius;         // ボールの半径
	unsigned int color;   // ボールの色
};

/// <summary>
/// ばね
/// </summary>
class Spring {
public:
	Spring();

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

private:
	// ばね
	Vector3 anchor_;           // アンカー。固定された端の位置
	float naturalLength_;      // 自然長
	float stiffness_;          // 剛性。バネ定数k
	float dampingCoefficient_; // 減衰係数

	// ボール
	Ball ball_;

	// 60fpsなのでdeltaTimeは1/60秒
	float deltaTime_ = 1.0f / 60.0f;

	// Startボタンが押されたら動き始める
	bool isStart_ = false;
};

#pragma endregion
