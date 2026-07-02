#include "Pendulum.h"
#include "Matrix4x4.h"
#include "Sphere.h"
#include <Novice.h>
#include <cmath>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region Pendulum

Pendulum::Pendulum() {
	// 振り子
	anchor_ = {0.0f, 1.0f, 0.0f}; // アンカーポイント
	length_ = 0.8f;               // 紐の長さ
	angle_ = 0.7f;                // 現在の角度
	angularVelocity_ = 0.0f;      // 角速度ω
	angularAcceleration_ = 0.0f;  // 角加速度

	// 先端の球
	// アンカーから角度angle_・長さlength_だけ振れた位置が振り子の先端
	position_.x = anchor_.x + std::sin(angle_) * length_;
	position_.y = anchor_.y - std::cos(angle_) * length_;
	position_.z = anchor_.z;
	ballRadius_ = 0.05f;
	color_ = WHITE;
}

void Pendulum::Update() {
	// Startボタンが押されるまでは動かさない
	if (!isStart_) {
		return;
	}

	// 角加速度を求める。重力加速度9.8[m/s^2]による復元の運動
	angularAcceleration_ = -(9.8f / length_) * std::sin(angle_);
	// 角加速度・角速度は秒を基準とした値なので、deltaTime(1/60秒)分だけ適用する
	angularVelocity_ += angularAcceleration_ * deltaTime_;
	angle_ += angularVelocity_ * deltaTime_;

	// 振り子の先端の位置を求める
	position_.x = anchor_.x + std::sin(angle_) * length_;
	position_.y = anchor_.y - std::cos(angle_) * length_;
	position_.z = anchor_.z;
}

void Pendulum::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	// アンカーと先端をスクリーン座標に変換
	Vector3 anchorScreen = Transform(Transform(anchor_, viewProjectionMatrix), viewportMatrix);
	Vector3 tipScreen = Transform(Transform(position_, viewProjectionMatrix), viewportMatrix);

	// 紐(線)を描画
	Novice::DrawLine((int)anchorScreen.x, (int)anchorScreen.y, (int)tipScreen.x, (int)tipScreen.y, WHITE);

	// 先端の球を描画
	Sphere ball(position_, ballRadius_, color_);
	ball.Draw(viewProjectionMatrix, viewportMatrix);
}

#ifdef _DEBUG
void Pendulum::DrawImGui() {
	ImGui::DragFloat3("Anchor", &anchor_.x, 0.01f);
	ImGui::DragFloat("Length", &length_, 0.01f);
	ImGui::DragFloat("Angle", &angle_, 0.01f);
	ImGui::DragFloat("AngularVelocity", &angularVelocity_, 0.01f);
	ImGui::DragFloat("AngularAcceleration", &angularAcceleration_, 0.01f);
	if (ImGui::Button("Start")) {
		isStart_ = true;
	}
	ImGui::Separator();
}
#endif

#pragma endregion
