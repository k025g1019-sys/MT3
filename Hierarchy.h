#pragma once
#include <array>
#include "Matrix4x4.h"

class Hierarchy {
public:
	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

#ifdef _DEBUG
	void DrawImGui();
#endif

private:
	std::array<Vector3, 3> translates_ = {
	    Vector3{0.2f, 1.0f, 0.0f},
	    Vector3{0.4f, 0.0f, 0.0f},
	    Vector3{0.3f, 0.0f, 0.0f},
	};

	std::array<Vector3, 3> rotates_ = {
	    Vector3{0.0f, 0.0f, -6.8f},
	    Vector3{0.0f, 0.0f, -1.4f},
	    Vector3{0.0f, 0.0f, 0.0f },
	};

	std::array<Vector3, 3> scales_ = {
	    Vector3{1.0f, 1.0f, 1.0f},
	    Vector3{1.0f, 1.0f, 1.0f},
	    Vector3{1.0f, 1.0f, 1.0f},
	};

	Matrix4x4 localMatrices_[3];
	Matrix4x4 worldMatrices_[3];
};