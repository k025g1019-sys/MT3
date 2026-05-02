#include "Plane.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include <cmath>

#pragma region Plane

Vector3 Plane::Perpendicular(const Vector3& vector) const {
	if (fabs(vector.x) > fabs(vector.y)) {
		return {-vector.y, vector.x, 0.0f};
	}
	return {0.0f, -vector.z, vector.y};
}

void Plane::Update() {};

void Plane::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	Vector3 center = Multiply(distance_, normal_);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(normal_));
	perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z};
	perpendiculars[2] = Normalize(Cross(normal_, perpendiculars[0]));
	perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z};
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(perpendiculars[index], 2.0f);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), 0xFFFFFFFF);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), 0xFFFFFFFF);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), 0xFFFFFFFF);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), 0xFFFFFFFF);
}

#ifdef _DEBUG
#include <imgui.h>
void Plane::DrawImGui() {
	ImGui::DragFloat3("Normal", &normal_.x, 0.01f);
	ImGui::DragFloat("Distance", &distance_, 0.01f);
	ImGui::Separator();
}
#endif

void Plane::SetNormal(const Vector3& p) { normal_ = Normalize(p); } // 正規化する

#pragma endregion
