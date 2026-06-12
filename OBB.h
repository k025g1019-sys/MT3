#pragma once
#include <array>
#include "Vector3.h"

struct Matrix4x4;

#pragma region OBB

/// <summary>
/// OBB
/// </summary>
/// <param name="center">中心座標</param>
/// <param name="rotate">回転</param>
/// <param name="orientations">ローカル軸（正規化・直交必須）</param>
/// <param name="size">各軸方向の半サイズ</param>
class OBB {
public:
	OBB() = default;

	OBB(const Vector3& center, const Vector3& rotate, const std::array<Vector3, 3>& orientations, const Vector3& size, unsigned int color = 0xFFFFFFFF)
	    : center_(center), rotate_(rotate), orientations_(orientations), size_(size), color_(color) {

		orientations_[0] = orientations[0];
		orientations_[1] = orientations[1];
		orientations_[2] = orientations[2];
	}

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

#ifdef _DEBUG
	void DrawImGui();
#endif

	void Normalize();

	const Vector3& GetCenter() const { return center_; }
	const Vector3& GetRotate() const { return rotate_; }
	const Vector3& GetSize() const { return size_; }

	const std::array<Vector3, 3>& GetOrientations() const { return orientations_; }

	void SetCenter(const Vector3& center) { center_ = center; }
	void SetRotate(const Vector3& rotate) { rotate_ = rotate; }
	void SetSize(const Vector3& size) { size_ = size; }

	void SetOrientation(int index, const Vector3& axis) {
		if (index >= 0 && index < 3) {
			orientations_[index] = axis;
		}
	}

	void SetHit(bool hit) { isHit_ = hit; }
	bool IsHit() const { return isHit_; }

	void SetColor(unsigned int p) { color_ = p; }

private:
	Vector3 center_{0.0f, 0.0f, 0.0f};
	Vector3 rotate_{0.0f, 0.0f, 0.0f}; // ラジアン

	// ローカル軸
	std::array<Vector3, 3> orientations_ = {
	    Vector3{1.0f, 0.0f, 0.0f},
        Vector3{0.0f, 1.0f, 0.0f},
        Vector3{0.0f, 0.0f, 1.0f}
    };

	// 半サイズ
	Vector3 size_{0.5f, 0.5f, 0.5f};

	bool isHit_ = false;
	unsigned int color_{0xFFFFFFFF};
};

#pragma endregion