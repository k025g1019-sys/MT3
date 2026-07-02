#define _USE_MATH_DEFINES
#include "CircularMotion.h"
#include "Matrix4x4.h"
#include "Sphere.h"
#include <Novice.h>
#include <cmath>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region CircularMotion

CircularMotion::CircularMotion() {
	// 円
	center_ = {0.0f, 0.0f, 0.0f};
	radius_ = 0.8f;                 // 半径0.8m
	angularVelocity_ = float(M_PI); // 角速度π[rad/s] → 2秒で1周
	angle_ = 0.0f;

	// 球(角度0のとき cos0=1, sin0=0 なので中心から+x方向へ半径分の位置)
	position_ = {center_.x + radius_, center_.y, center_.z};
	ballRadius_ = 0.05f;
	color_ = WHITE;
}

void CircularMotion::Update() {
	// Startボタンが押されるまでは動かさない
	if (!isStart_) {
		return;
	}

	// 角速度は角度の速度なので、角度に対して毎フレーム加算する
	// 単位はrad/sなので、deltaTimeをかけて1/60秒分だけ加算する
	angle_ += angularVelocity_ * deltaTime_;

	// xy平面上での円運動として、球の位置を求める
	position_.x = center_.x + std::cos(angle_) * radius_;
	position_.y = center_.y + std::sin(angle_) * radius_;
	position_.z = center_.z;
}

void CircularMotion::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	// 球を描画
	Sphere ball(position_, ballRadius_, color_);
	ball.Draw(viewProjectionMatrix, viewportMatrix);
}

#ifdef _DEBUG
void CircularMotion::DrawImGui() {
	ImGui::DragFloat3("Center", &center_.x, 0.01f);
	ImGui::DragFloat("Radius", &radius_, 0.01f);
	ImGui::DragFloat("AngularVelocity", &angularVelocity_, 0.01f);
	ImGui::DragFloat("Angle", &angle_, 0.01f);
	if (ImGui::Button("Start")) {
		isStart_ = true;
	}
	ImGui::Separator();
}
#endif

#pragma endregion
