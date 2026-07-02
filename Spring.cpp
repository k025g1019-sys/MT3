#include <Novice.h>
#include "Matrix4x4.h"
#include "Sphere.h"
#include "Spring.h"
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region Spring

Spring::Spring() {
	// ばね
	anchor_ = {0.0f, 0.0f, 0.0f};
	naturalLength_ = 1.0f;
	stiffness_ = 100.0f;
	dampingCoefficient_ = 2.0f;

	// ボール
	ball_.position = {1.2f, 0.0f, 0.0f};
	ball_.velocity = {0.0f, 0.0f, 0.0f};
	ball_.acceleration = {0.0f, 0.0f, 0.0f};
	ball_.mass = 2.0f;
	ball_.radius = 0.05f;
	ball_.color = BLUE;
}

void Spring::Update() {
	// Startボタンが押されるまでは動かさない
	if (!isStart_) {
		return;
	}

	Vector3 diff = ball_.position - anchor_;
	float length = Length(diff);
	if (length != 0.0f) {
		Vector3 direction = Normalize(diff);
		Vector3 restPosition = anchor_ + direction * naturalLength_;
		// フックの法則 F = -kx の変位ベクトルx。自然長の位置(restPosition)から現在位置までのずれ
		Vector3 displacement = ball_.position - restPosition;
		Vector3 restoringForce = -stiffness_ * displacement;
		// 減衰抵抗を計算する
		Vector3 dampingForce = -dampingCoefficient_ * ball_.velocity;
		// 減衰抵抗も加味して、物体にかかる力を決定する
		Vector3 force = restoringForce + dampingForce;
		ball_.acceleration = force / ball_.mass;
	}
	// 加速度も速度もどちらも秒を基準とした値である
	// それが、1/60秒間(deltaTime)適用されたと考える
	ball_.velocity += ball_.acceleration * deltaTime_;
	ball_.position += ball_.velocity * deltaTime_;
}

void Spring::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	// アンカーとボールをスクリーン座標に変換
	Vector3 anchorScreen = Transform(Transform(anchor_, viewProjectionMatrix), viewportMatrix);
	Vector3 ballScreen = Transform(Transform(ball_.position, viewProjectionMatrix), viewportMatrix);

	// ばね(線)を描画
	Novice::DrawLine((int)anchorScreen.x, (int)anchorScreen.y, (int)ballScreen.x, (int)ballScreen.y, WHITE);

	// ボール(球)を描画
	Sphere ball(ball_.position, ball_.radius, ball_.color);
	ball.Draw(viewProjectionMatrix, viewportMatrix);
}

#ifdef _DEBUG
void Spring::DrawImGui() {
	ImGui::DragFloat3("Anchor", &anchor_.x, 0.01f);
	ImGui::DragFloat("NaturalLength", &naturalLength_, 0.01f);
	ImGui::DragFloat("Stiffness", &stiffness_, 0.01f);
	ImGui::DragFloat("DampingCoefficient", &dampingCoefficient_, 0.01f);
	ImGui::DragFloat3("Ball Position", &ball_.position.x, 0.01f);
	if (ImGui::Button("Start")) {
		isStart_ = true;
	}
	ImGui::Separator();
}
#endif

#pragma endregion
