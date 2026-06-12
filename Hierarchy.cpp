#include <Novice.h>
#include "Hierarchy.h"
#include "Sphere.h"
#include <imgui.h>

void Hierarchy::Update() {
	for (int i = 0; i < 3; i++) {

		localMatrices_[i] = MakeAffineMatrix(scales_[i], rotates_[i], translates_[i]);
	}

	worldMatrices_[0] = localMatrices_[0];

	worldMatrices_[1] = Multiply(localMatrices_[1], worldMatrices_[0]);

	worldMatrices_[2] = Multiply(localMatrices_[2], worldMatrices_[1]);
}

void Hierarchy::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	// ワールド座標を取得
	Vector3 joints[3];

	for (int i = 0; i < 3; i++) {

		joints[i] = {worldMatrices_[i].m[3][0], worldMatrices_[i].m[3][1], worldMatrices_[i].m[3][2]};
	}

	// 関節の球
	// 肩
	Sphere shoulder(joints[0], 0.05f, 0xFF0000FF);
	// 肘
	Sphere elbow(joints[1], 0.05f, 0x00FF00FF);
	// 手
	Sphere hand(joints[2], 0.05f, 0x0000FFFF);

	// 関節線
	// スクリーン変換
	Vector3 s0 = Transform(Transform(joints[0], viewProjectionMatrix), viewportMatrix);
	Vector3 s1 = Transform(Transform(joints[1], viewProjectionMatrix), viewportMatrix);
	Vector3 s2 = Transform(Transform(joints[2], viewProjectionMatrix), viewportMatrix);
	
	// 球描画
	shoulder.Draw(viewProjectionMatrix, viewportMatrix);
	elbow.Draw(viewProjectionMatrix, viewportMatrix);
	hand.Draw(viewProjectionMatrix, viewportMatrix);
	// 関節線描画
	Novice::DrawLine((int)s0.x, (int)s0.y, (int)s1.x, (int)s1.y, WHITE);
	Novice::DrawLine((int)s1.x, (int)s1.y, (int)s2.x, (int)s2.y, WHITE);
}

void Hierarchy::DrawImGui() {
	if (ImGui::TreeNode("Shoulder")) {

		ImGui::DragFloat3("Translate", &translates_[0].x, 0.01f);

		ImGui::DragFloat3("Rotate", &rotates_[0].x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Elbow")) {

		ImGui::DragFloat3("Translate", &translates_[1].x, 0.01f);

		ImGui::DragFloat3("Rotate", &rotates_[1].x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Hand")) {

		ImGui::DragFloat3("Translate", &translates_[2].x, 0.01f);

		ImGui::DragFloat3("Rotate", &rotates_[2].x, 0.01f);

		ImGui::TreePop();
	}
	ImGui::Separator();
}