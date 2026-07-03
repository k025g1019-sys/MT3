#include "ConicalPendulum.h"
#include "Matrix4x4.h"
#include "Sphere.h"
#include <Novice.h>
#include <cmath>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region ConicalPendulum

ConicalPendulum::ConicalPendulum() {
	// 円錐振り子
	anchor_ = {0.0f, 1.0f, 0.0f}; // アンカーポイント
	length_ = 0.8f;               // 紐の長さ
	halfApexAngle_ = 0.7f;        // 円錐の頂角の半分
	angle_ = 0.0f;                // 現在の角度
	angularVelocity_ = 0.0f;      // 角速度ω

	// 先端の球(ボブ)。角度・半径・高さからボブの位置を求める
	float radius = std::sin(halfApexAngle_) * length_;
	float height = std::cos(halfApexAngle_) * length_;
	position_.x = anchor_.x + std::cos(angle_) * radius;
	position_.y = anchor_.y - height;
	position_.z = anchor_.z - std::sin(angle_) * radius;
	ballRadius_ = 0.05f;
	color_ = WHITE;
}

void ConicalPendulum::Update() {
	// Startボタンが押されるまでは動かさない
	if (!isStart_) {
		return;
	}

	// 角速度を計算し、現在の角度に加算していく
	angularVelocity_ = std::sqrt(9.8f / (length_ * std::cos(halfApexAngle_)));
	angle_ += angularVelocity_ * deltaTime_;

	// 角度が分かれば、半径と高さから、ボブの位置が分かる
	float radius = std::sin(halfApexAngle_) * length_;
	float height = std::cos(halfApexAngle_) * length_;
	position_.x = anchor_.x + std::cos(angle_) * radius;
	position_.y = anchor_.y - height;
	position_.z = anchor_.z - std::sin(angle_) * radius;
}

void ConicalPendulum::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	// アンカーとボブをスクリーン座標に変換
	Vector3 anchorScreen = Transform(Transform(anchor_, viewProjectionMatrix), viewportMatrix);
	Vector3 ballScreen = Transform(Transform(position_, viewProjectionMatrix), viewportMatrix);

	// 紐(線)を描画
	Novice::DrawLine((int)anchorScreen.x, (int)anchorScreen.y, (int)ballScreen.x, (int)ballScreen.y, WHITE);

	// ボブ(球)を描画
	Sphere ball(position_, ballRadius_, color_);
	ball.Draw(viewProjectionMatrix, viewportMatrix);
}

#ifdef _DEBUG
void ConicalPendulum::DrawImGui() {
	ImGui::DragFloat3("Anchor", &anchor_.x, 0.01f);
	ImGui::DragFloat("Length", &length_, 0.01f);
	ImGui::DragFloat("HalfApexAngle", &halfApexAngle_, 0.01f);
	ImGui::DragFloat("Angle", &angle_, 0.01f);
	ImGui::DragFloat("AngularVelocity", &angularVelocity_, 0.01f);
	if (ImGui::Button("Start")) {
		isStart_ = true;
	}
	ImGui::Separator();
}
#endif

#pragma endregion
