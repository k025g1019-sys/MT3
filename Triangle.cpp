#include "Matrix4x4.h"
#include "Triangle.h"
#include <Novice.h>
#include <cmath>

#pragma region Triangle

void Triangle::Update() {}

void Triangle::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	// 頂点を変換
	Vector3 screenVertices[3];
	for (uint32_t i = 0; i < 3; ++i) {
		screenVertices[i] =
			Transform(
				Transform(vertices_[i], viewProjectionMatrix),
				viewportMatrix
			);
	}

	// 描画
	Novice::DrawTriangle(
		static_cast<int>(screenVertices[0].x), static_cast<int>(screenVertices[0].y),
		static_cast<int>(screenVertices[1].x), static_cast<int>(screenVertices[1].y),
		static_cast<int>(screenVertices[2].x), static_cast<int>(screenVertices[2].y),
		0xFFFFFFFF,
		kFillModeWireFrame
	);

}

#ifdef _DEBUG
#include <imgui.h>
void Triangle::DrawImGui() {
	ImGui::DragFloat3("vertices0", &vertices_[0].x, 0.01f);
	ImGui::DragFloat3("vertices1", &vertices_[1].x, 0.01f);
	ImGui::DragFloat3("vertices2", &vertices_[2].x, 0.01f);
	ImGui::Separator();
}
#endif

#pragma endregion
