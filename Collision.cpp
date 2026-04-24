#include "Vector3.h"
#include "Matrix4x4.h"
#include <cmath>
#include "Structure.h"
#include "Collision.h"

// 2点間の距離を求める
float Length(const Vector3& center1, const Vector3& center2) {
	return std::sqrt((center2.x - center1.x) * (center2.x - center1.x) + (center2.y - center1.y) * (center2.y - center1.y) + (center2.z - center1.z) * (center2.z - center1.z));
}

// 球同士の当たり判定
bool IsSphereSphereCollision(const Sphere& s1, const Sphere& s2) {
	// 2つの球の中心点間の距離を求める
	double distance = Length(s1.GetCenter(), s2.GetCenter());
	return distance <= s1.GetRadius() + s2.GetRadius();
}

// 球と平面の衝突判定
bool IsSpherePlaneCollision(const Sphere& sphere, const Plane& plane) {
	Vector3 center = sphere.GetCenter();

	// 平面の法線
	Vector3 normal = Normalize(plane.GetNormal());
	float d = plane.GetDistance(); // ax + by + cz + d の d

	// 点と平面の距離
	// 平面上の1点（描画と一致させる）
	Vector3 planePoint = Multiply(d, normal);

	// 球中心 → 平面へのベクトル
	Vector3 diff = Subtract(center, planePoint);

	// 法線方向の距離（符号付き）
	float distance = std::abs(Dot(diff, normal));

	return distance <= sphere.GetRadius();
}