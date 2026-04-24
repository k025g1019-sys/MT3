#pragma once
#include "Vector3.h"

struct Matrix4x4;

class Camera {
public:
	void Update(const int kWindowWidth, const int kWindowHeight, Matrix4x4& viewProjectionMatrix, Matrix4x4& viewportMatrix, const char* keys);

	Vector3 GetPosition() const { return position; }
	Vector3 GetRotation() const { return rotate; }
	float GetMoveSpeed() const { return moveSpeed; }
	float GetRotateSpeed() const { return rotateSpeed; }

	void SetPosition(const Vector3& p) { position = p; }
	void SetRotation(Vector3 r) { rotate = r; }

private:
	// カメラの初期位置, 角度
	Vector3 position{0.0f, 1.9f, -6.49f};
	Vector3 rotate{0.26f, 0.0f, 0.0f};
	// カメラ移動速度
	float moveSpeed = 0.04f;
	float rotateSpeed = 0.01f;
};