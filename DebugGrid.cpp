#include "DebugGrid.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include <cmath>

#pragma region Grid

// グリッド表示関数
// Red : z
// Blue : x
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;                                      // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                       // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ

	// ビュー射影→ビューポートの合成行列（v * VP * Viewport）
	const Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

	// 奥から手前への線（X一定でZ方向に伸びる線）
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + kGridEvery * static_cast<float>(xIndex);

		// ワールド座標系上の始点と終点
		Vector3 worldStart{x, 0.0f, -kGridHalfWidth};
		Vector3 worldEnd{x, 0.0f, kGridHalfWidth};

		// スクリーン座標系まで変換
		Vector3 screenStart = Transform(worldStart, vpvMatrix);
		Vector3 screenEnd = Transform(worldEnd, vpvMatrix);

		// 原点を通る線は別の色にする
		unsigned int color = (std::fabs(x) < 1e-4f) ? 0xFF0000FF : 0xAAAAAAFF;

		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);
	}

	// 左右方向の線（Z一定でX方向に伸びる線）
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + kGridEvery * static_cast<float>(zIndex);

		Vector3 worldStart{-kGridHalfWidth, 0.0f, z};
		Vector3 worldEnd{kGridHalfWidth, 0.0f, z};

		Vector3 screenStart = Transform(worldStart, vpvMatrix);
		Vector3 screenEnd = Transform(worldEnd, vpvMatrix);

		unsigned int color = (std::fabs(z) < 1e-4f) ? 0x0000FFFF : 0xAAAAAAFF;

		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);
	}

	// 原点から上に伸びる高さ方向（Y軸）の線
	// ワールド座標（原点から上方向へ）
	Vector3 worldStartY{0.0f, 0.0f, 0.0f};
	// Vector3 worldStartY{0.0f, -kGridHalfWidth, 0.0f};
	Vector3 worldEndY{0.0f, kGridHalfWidth, 0.0f};

	// スクリーン座標へ変換
	Vector3 screenStart = Transform(worldStartY, vpvMatrix);
	Vector3 screenEnd = Transform(worldEndY, vpvMatrix);

	// 緑色（RGBA）
	unsigned int color = 0x00FF00FF;

	Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);
}

#pragma endregion

#pragma region DebugScreenPrintf

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

#pragma endregion
