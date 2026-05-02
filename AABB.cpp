#include "AABB.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include <algorithm>

#pragma region AABB

void AABB::Normalize() {
	Vector3 newMin{(std::min)(min_.x, max_.x), (std::min)(min_.y, max_.y), (std::min)(min_.z, max_.z)};
	Vector3 newMax{(std::max)(min_.x, max_.x), (std::max)(min_.y, max_.y), (std::max)(min_.z, max_.z)};

	min_ = newMin;
	max_ = newMax;
}

void AABB::Update() {
	//Normalize();
}

void AABB::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	AABB world = GetWorldAABB();

	Vector3 min = world.GetMin();
	Vector3 max = world.GetMax();

	// 8頂点（AABBローカル）
	Vector3 v[8] = {
	    {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},

	    {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {min.x, max.y, max.z},
        {max.x, max.y, max.z},
	};

	// v' = v * M
	Vector3 sv[8];
	for (int i = 0; i < 8; i++) {
		Vector3 clip = Transform(v[i], viewProjectionMatrix); // v * VP
		sv[i] = Transform(clip, viewportMatrix);              // v * VP * VPt
	}

	auto drawLine = [&](int i, int j) { Novice::DrawLine((int)sv[i].x, (int)sv[i].y, (int)sv[j].x, (int)sv[j].y, color_); };

	// 前面
	drawLine(0, 1);
	drawLine(1, 3);
	drawLine(3, 2);
	drawLine(2, 0);

	// 背面
	drawLine(4, 5);
	drawLine(5, 7);
	drawLine(7, 6);
	drawLine(6, 4);

	// 接続
	drawLine(0, 4);
	drawLine(1, 5);
	drawLine(2, 6);
	drawLine(3, 7);
}

#ifdef _DEBUG
#include <imgui.h>
void AABB::DrawImGui() {
	ImGui::DragFloat3("Min", &min_.x, 0.01f);
	ImGui::DragFloat3("Max", &max_.x, 0.01f);
	ImGui::Separator();
}
#endif

#pragma endregion
