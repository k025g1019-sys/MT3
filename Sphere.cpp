#define _USE_MATH_DEFINES
#include "Sphere.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include <cmath>

#pragma region Sphere

void Sphere::Update() {}

// 球を描画する
void Sphere::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 12;                                 // 分割数
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

#ifdef _DEBUG
#include <imgui.h>
void Sphere::DrawImGui() {
	ImGui::DragFloat3("Center", &center_.x, 0.01f);
	ImGui::DragFloat("Radius", &radius_, 0.01f);
	ImGui::Separator();
}
#endif

#pragma endregion
