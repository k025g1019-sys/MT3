#pragma once
#include "Vector3.h"

struct Matrix4x4;

#pragma region Triangle

/// <summary>
/// 三角形
/// </summary>
/// <param name="Vector3  vertices">頂点</param>
class Triangle {
public:
	Triangle() = default;
	Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
		vertices_[0] = v0;
		vertices_[1] = v1;
		vertices_[2] = v2;
	}

	void Update();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImGui();
#endif

	const Vector3* GetVertices() const { return vertices_; }

	void SetVertex(int index, const Vector3& p) {
		if (index >= 0 && index < 3) {
			vertices_[index] = p;
		}
	}

private:
	Vector3 vertices_[3]; //!< 頂点
};

#pragma endregion
