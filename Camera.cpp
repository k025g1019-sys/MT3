#include "Vector3.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include "Camera.h"
#ifdef _DEBUG
#include <imgui.h>
#endif

void Camera::Update(const int kWindowWidth, const int kWindowHeight, Matrix4x4& viewProjectionMatrix, Matrix4x4& viewportMatrix, const char* keys) {

	// WASDキーでカメラ移動
	if (keys[DIK_D] != keys[DIK_A]) {
		position.x += keys[DIK_D] ? moveSpeed : -moveSpeed;
	}
	if (keys[DIK_W] != keys[DIK_S]) {
		position.y += keys[DIK_W] ? moveSpeed : -moveSpeed;
	}
	if (keys[DIK_E] != keys[DIK_Q]) {
		position.z += keys[DIK_E] ? moveSpeed * 2.0f : -moveSpeed * 2.0f;
	}

	// カメラの回転
	if (keys[DIK_U] != keys[DIK_J]) {
		rotate.x += keys[DIK_U] ? rotateSpeed : -rotateSpeed;
	}
	if (keys[DIK_K] != keys[DIK_H]) {
		rotate.y += keys[DIK_K] ? rotateSpeed : -rotateSpeed;
	}
	if (keys[DIK_I] != keys[DIK_Y]) {
		rotate.z += keys[DIK_I] ? rotateSpeed : -rotateSpeed;
	}

	// カメラの回転行列（逆回転）
	Matrix4x4 cameraRotX = MakeRotateXMatrix(-rotate.x);
	Matrix4x4 cameraRotY = MakeRotateYMatrix(-rotate.y);
	Matrix4x4 cameraRotZ = MakeRotateZMatrix(-rotate.z);

	// カメラの平行移動（逆方向）
	Matrix4x4 cameraTrans = MakeTranslateMatrix({-position.x, -position.y, -position.z});

	// ビュー行列 = R^-1 * T^-1
	Matrix4x4 viewMatrix = Multiply(Multiply(cameraRotX, cameraRotY), Multiply(cameraRotZ, cameraTrans));

	// 各種行列の計算
	// 行列の計算・変換は描画の直前に置くのが良い
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
	viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
}