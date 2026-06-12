#pragma once
#include <assert.h>
#include <cmath>
#include <Novice.h>
#include "Matrix4x4.h"
#include "Sphere.h"
#include "Curve.h"

#pragma region Curve

void Curve::Update() {}

Vector3 Curve::Lerp(const Vector3& v1, const Vector3& v2, float t) {
	
	return {
		v1.x + (v2.x - v1.x) * t,
		v1.y + (v2.y - v1.y) * t,
		v1.z + (v2.z - v1.z) * t
	};
}

void Curve::DrawBezier(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	const uint32_t kSubdivision = 100;

	for (uint32_t i = 0; i < kSubdivision; i++) {

		float t0 = float(i) / float(kSubdivision);
		float t1 = float(i + 1) / float(kSubdivision);

		// ---- t0 ----
		Vector3 p01_0 = Lerp(controlPoints_[0], controlPoints_[1], t0);
		Vector3 p12_0 = Lerp(controlPoints_[1], controlPoints_[2], t0);
		Vector3 bezier0 = Lerp(p01_0, p12_0, t0);

		// ---- t1 ----
		Vector3 p01_1 = Lerp(controlPoints_[0], controlPoints_[1], t1);
		Vector3 p12_1 = Lerp(controlPoints_[1], controlPoints_[2], t1);
		Vector3 bezier1 = Lerp(p01_1, p12_1, t1);

		// ViewProjection
		Vector3 screen0 = Transform(Transform(bezier0, viewProjectionMatrix), viewportMatrix);

		Vector3 screen1 = Transform(Transform(bezier1, viewProjectionMatrix), viewportMatrix);

		Novice::DrawLine(int(screen0.x), int(screen0.y), int(screen1.x), int(screen1.y), color_);
	}
}

void Curve::DrawControlPoints(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	for (int i = 0; i < 3; i++) {
		Sphere point(controlPoints_[i], 0.01f, 0x000000FF);
		point.Draw(viewProjectionMatrix, viewportMatrix);
	}
}

void Curve::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	DrawBezier(viewProjectionMatrix, viewportMatrix);

	if (isDrawControlPoints_) {
		DrawControlPoints(viewProjectionMatrix, viewportMatrix);
	}
}

#ifdef _DEBUG
#include <imgui.h>
void Curve::DrawImGui() {
	ImGui::DragFloat3("controlPoints[0]", &controlPoints_[0].x, 0.01f);
	ImGui::DragFloat3("controlPoints[1]", &controlPoints_[1].x, 0.01f);
	ImGui::DragFloat3("controlPoints[2]", &controlPoints_[2].x, 0.01f);
	ImGui::Checkbox("Draw Control Points", &isDrawControlPoints_);
	ImGui::Separator();
}
#endif

#pragma endregion