#define _USE_MATH_DEFINES
#include "Sphere.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include <Novice.h>
#include <cmath>

#pragma region Sphere

void Sphere::Update() {
	// 1フレーム前の位置を保存する(スイープ判定用)
	prevCenter_ = center_;

	// 重力が無効、またはStartボタンが押されるまでは動かさない
	if (!enableGravity_ || !isStart_) {
		return;
	}

	// 速度と位置を更新する
	velocity_ += acceleration_ * deltaTime_;
	center_ += velocity_ * deltaTime_;
}

void Sphere::BounceOffPlane(const Plane& plane) {
	const Vector3 normal = plane.GetNormal();

	// 平面のどちら側にいるか(1フレーム前の位置を基準に決める)
	float prevDistance = Dot(prevCenter_, normal) - plane.GetDistance();
	float sign = (prevDistance >= 0.0f) ? 1.0f : -1.0f;

	// めり込んだ分だけ法線方向に押し戻す
	// (移動区間ごと巻き戻すと接線方向の移動まで消えて、球がその場で停止してしまう)
	float distance = Dot(center_, normal) - plane.GetDistance();
	if (sign * distance < radius_) {
		center_ += normal * (sign * radius_ - distance);
	}

	// 平面に向かっているときだけ反射する
	// (離れていく最中にも反射すると、速度が毎フレーム反転して平面に張り付いてしまう)
	if (sign * Dot(velocity_, normal) < 0.0f) {
		// 反射ベクトルを求め、速度を反射方向に変更する
		Vector3 reflected = Reflect(velocity_, normal);
		Vector3 projectToNormal = Project(reflected, normal);
		Vector3 movingDirection = reflected - projectToNormal;
		// 反発係数による減衰を法線方向にだけ入れる
		velocity_ = projectToNormal * restitution_ + movingDirection;
	}
}

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
	ImGui::Checkbox("EnableGravity", &enableGravity_);
	if (enableGravity_) {
		ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);
		ImGui::DragFloat("Mass", &mass_, 0.01f);
		ImGui::DragFloat("Restitution", &restitution_, 0.01f, 0.0f, 1.0f);
		if (ImGui::Button("Start")) {
			isStart_ = true;
			acceleration_ = {0.0f, -9.8f, 0.0f}; // 重力加速度
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			isStart_ = false;
			center_ = initialCenter_;
			prevCenter_ = initialCenter_;
			velocity_ = initialVelocity_;
		}
	}
	ImGui::Separator();
}
#endif

#pragma endregion
