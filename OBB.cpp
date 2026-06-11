#include "OBB.h"
#include "Matrix4x4.h"

#include <Novice.h>
#include <algorithm>

#pragma region OBB

//----------------------------------------
// ベクトル正規化
//----------------------------------------
static Vector3 NormalizeVector(const Vector3& v) {

	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	if (length == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}

	return {v.x / length, v.y / length, v.z / length};
}

//----------------------------------------
// OBB軸正規化
//----------------------------------------
void OBB::Normalize() {

	// X軸
	Vector3 x = NormalizeVector(orientations_[0]);

	// Y軸からX方向成分を除去
	Vector3 y = orientations_[1];

	y = Subtract(y, Multiply(Dot(y, x), x));
	y = NormalizeVector(y);

	// Z軸は外積で生成
	Vector3 z = Cross(x, y);
	z = NormalizeVector(z);

	orientations_[0] = x;
	orientations_[1] = y;
	orientations_[2] = z;

	// 半サイズは正に
	size_.x = std::abs(size_.x);
	size_.y = std::abs(size_.y);
	size_.z = std::abs(size_.z);
}

Matrix4x4 MakeRotateXYZMatrix(const Vector3& r) {

	Matrix4x4 rx = MakeRotateXMatrix(r.x);
	Matrix4x4 ry = MakeRotateYMatrix(r.y);
	Matrix4x4 rz = MakeRotateZMatrix(r.z);

	return Multiply(rx, Multiply(ry, rz));
}

void OBB::Update() {

	Matrix4x4 rotateMatrix = MakeRotateXYZMatrix(rotate_);

	orientations_[0] = {rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2]};

	orientations_[1] = {rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2]};

	orientations_[2] = {rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2]};

	Normalize();
}

//----------------------------------------
// 描画
//----------------------------------------
void OBB::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	// 各軸 * 半サイズ
	Vector3 xAxis = orientations_[0] * size_.x;
	Vector3 yAxis = orientations_[1] * size_.y;
	Vector3 zAxis = orientations_[2] * size_.z;

	//----------------------------------------
	// 8頂点生成
	//----------------------------------------
	Vector3 v[8];

	v[0] = center_ - xAxis - yAxis - zAxis;
	v[1] = center_ + xAxis - yAxis - zAxis;
	v[2] = center_ - xAxis + yAxis - zAxis;
	v[3] = center_ + xAxis + yAxis - zAxis;

	v[4] = center_ - xAxis - yAxis + zAxis;
	v[5] = center_ + xAxis - yAxis + zAxis;
	v[6] = center_ - xAxis + yAxis + zAxis;
	v[7] = center_ + xAxis + yAxis + zAxis;

	//----------------------------------------
	// スクリーン変換
	//----------------------------------------
	Vector3 sv[8];

	for (int i = 0; i < 8; i++) {

		Vector3 clip = Transform(v[i], viewProjectionMatrix);

		sv[i] = Transform(clip, viewportMatrix);
	}

	auto drawLine = [&](int i, int j) { Novice::DrawLine((int)sv[i].x, (int)sv[i].y, (int)sv[j].x, (int)sv[j].y, color_); };

	//----------------------------------------
	// 前面
	//----------------------------------------
	drawLine(0, 1);
	drawLine(1, 3);
	drawLine(3, 2);
	drawLine(2, 0);

	//----------------------------------------
	// 背面
	//----------------------------------------
	drawLine(4, 5);
	drawLine(5, 7);
	drawLine(7, 6);
	drawLine(6, 4);

	//----------------------------------------
	// 接続
	//----------------------------------------
	drawLine(0, 4);
	drawLine(1, 5);
	drawLine(2, 6);
	drawLine(3, 7);
}

#ifdef _DEBUG

#include <imgui.h>

void OBB::DrawImGui() {

	ImGui::DragFloat3("Center", &center_.x, 0.01f);
	ImGui::DragFloat3("Size", &size_.x, 0.01f);

	ImGui::Separator();

	ImGui::Text("Rotate");
	ImGui::DragFloat3("Rotate", &rotate_.x, 0.01f);

	ImGui::Separator();

	ImGui::Text("Orientation X");
	ImGui::DragFloat3("AxisX", &orientations_[0].x, 0.01f);

	ImGui::Text("Orientation Y");
	ImGui::DragFloat3("AxisY", &orientations_[1].x, 0.01f);

	ImGui::Separator();
}

#endif

#pragma endregion