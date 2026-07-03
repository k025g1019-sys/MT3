#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region ConicalPendulum

/// <summary>
/// 円錐振り子
/// </summary>
class ConicalPendulum {
public:
	ConicalPendulum();

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

private:
	// 円錐振り子
	Vector3 anchor_;        // アンカーポイント。固定された端の位置
	float length_;          // 紐の長さ
	float halfApexAngle_;   // 円錐の頂角の半分
	float angle_;           // 現在の角度
	float angularVelocity_; // 角速度ω

	// 先端の球(ボブ)
	Vector3 position_;   // ボブの位置
	float ballRadius_;   // 球の半径
	unsigned int color_; // 球の色

	// 60fpsなのでdeltaTimeは1/60秒
	float deltaTime_ = 1.0f / 60.0f;

	// Startボタンが押されたら動き始める
	bool isStart_ = false;
};

#pragma endregion
