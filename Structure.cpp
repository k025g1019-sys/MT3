#define _USE_MATH_DEFINES
#include "Structure.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include <Novice.h>
#include <cmath>

#pragma region Sphere

void Sphere::UpdateToKeyMove(const char* keys) {

	// 上下キーで球1の前後移動
	if (!keys[DIK_LSHIFT] && (keys[DIK_UP] != keys[DIK_DOWN])) {
		center_.z += keys[DIK_UP] ? moveSpeed_ : -moveSpeed_;
	}

	// 左右キーで球1の左右移動
	if (keys[DIK_RIGHT] != keys[DIK_LEFT]) {
		center_.x += keys[DIK_RIGHT] ? moveSpeed_ : -moveSpeed_;
	}

	// 左SHIFT + 上下キーで球1の高さ移動
	if (keys[DIK_LSHIFT] && (keys[DIK_UP] != keys[DIK_DOWN])) {
		center_.y += keys[DIK_UP] ? moveSpeed_ : -moveSpeed_;
	}
}

// 球を描画する
void Sphere::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 16;                                 // 分割数
	const float kLonEvery = 2.0f * float(M_PI) / float(kSubdivision); // 経度分割1つ分の角度
	const float kLatEvery = float(M_PI) / float(kSubdivision);        // 緯度分割1つ分の角度
	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // 現在の緯度
		float nextLat = lat + kLatEvery;
		// 経度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度
			float nextLon = lon + kLonEvery;
			// world座標系でのa,b,cを求める
			Vector3 a{center_.x + radius_ * cosf(lat) * cosf(lon), center_.y + radius_ * sinf(lat), center_.z + radius_ * cosf(lat) * sinf(lon)};
			Vector3 b{center_.x + radius_ * cosf(nextLat) * cosf(lon), center_.y + radius_ * sinf(nextLat), center_.z + radius_ * cosf(nextLat) * sinf(lon)};
			Vector3 c{center_.x + radius_ * cosf(lat) * cosf(nextLon), center_.y + radius_ * sinf(lat), center_.z + radius_ * cosf(lat) * sinf(nextLon)};
			// a,b,cをScreen座標系まで変換
			Vector3 aScreen = Transform(a, viewProjectionMatrix);
			aScreen = Transform(aScreen, viewportMatrix);

			Vector3 bScreen = Transform(b, viewProjectionMatrix);
			bScreen = Transform(bScreen, viewportMatrix);

			Vector3 cScreen = Transform(c, viewProjectionMatrix);
			cScreen = Transform(cScreen, viewportMatrix);
			// ab,bcで線を引く
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color_);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color_);
		}
	}
}

#pragma endregion

#pragma region Plane

Vector3 Plane::Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return {-vector.y, vector.x, 0.0f};
	}
	return {0.0f, -vector.z, vector.y};
}

void Plane::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	Vector3 center = Multiply(distance, normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(normal));
	perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z};
	perpendiculars[2] = Cross(normal, perpendiculars[0]);
	perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z};
	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(perpendiculars[index], 2.0f);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[2].x), static_cast<int>(points[2].y), 0xFFFFFFFF);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), 0xFFFFFFFF);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), 0xFFFFFFFF);
	Novice::DrawLine(static_cast<int>(points[3].x), static_cast<int>(points[3].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), 0xFFFFFFFF);
}

#pragma endregion

#pragma region ScreenPrintf関数

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

#pragma region Grid

// グリッド表示関数
// Red : z
// Blue : x
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;                                      // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                       // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ

	// ビュー射影→ビューポートの合成行列（v * VP * Viewport）
	Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

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
