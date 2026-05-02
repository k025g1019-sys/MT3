#include "Segment.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include <algorithm>
#include <cmath>

#pragma region Segment

void Segment::Update() {}

// 最近接点
Vector3 Segment::ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 v = Subtract(point, segment.origin_);
	float dotDD = Dot(segment.diff_, segment.diff_);
	if (dotDD < 1e-6f) {
		return segment.origin_; // diff がゼロベクトルの場合
	}

	float t = Dot(v, segment.diff_) / dotDD;

	// 線分なので 0～1 にクランプ
	t = std::clamp(t, 0.0f, 1.0f);

	return {segment.origin_.x + segment.diff_.x * t, segment.origin_.y + segment.diff_.y * t, segment.origin_.z + segment.diff_.z * t};
}

void Segment::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	// 終点 = 始点 + 差分
	Vector3 end = Add(origin_, diff_);

	// ワールド → スクリーン座標
	Vector3 originScreen = Transform(Transform(origin_, viewProjectionMatrix), viewportMatrix);
	Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

	// 描画
	Novice::DrawLine(static_cast<int>(originScreen.x), static_cast<int>(originScreen.y), static_cast<int>(endScreen.x), static_cast<int>(endScreen.y), color_);
}

#ifdef _DEBUG
#include <imgui.h>
void Segment::DrawImGui() {
	ImGui::DragFloat3("Origin", &origin_.x, 0.01f);
	ImGui::DragFloat3("Diff", &diff_.x, 0.01f);
	ImGui::Separator();
}
#endif

#pragma endregion
