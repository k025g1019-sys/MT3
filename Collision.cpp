#include "Collision.h"
#include "AABB.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "Segment.h"
#include "Sphere.h"
#include "Triangle.h"
#include <algorithm>
#include <cmath>

#pragma region Length
// 2点間の距離を求める
float Length(const Vector3& center1, const Vector3& center2) {
	return std::sqrt((center2.x - center1.x) * (center2.x - center1.x) + (center2.y - center1.y) * (center2.y - center1.y) + (center2.z - center1.z) * (center2.z - center1.z));
}
#pragma endregion

#pragma region Sphere Sphere
// 球同士の当たり判定
bool IsSphereSphereCollision(const Sphere& s1, const Sphere& s2) {
	// 2つの球の中心点間の距離を求める
	double distance = Length(s1.GetCenter(), s2.GetCenter());
	return distance <= s1.GetRadius() + s2.GetRadius();
}
#pragma endregion

#pragma region Sphere Plane
// 球と平面の衝突判定
bool IsSpherePlaneCollision(const Sphere& sphere, const Plane& plane) {
	Vector3 center = sphere.GetCenter();

	// 平面の法線
	Vector3 normal = plane.GetNormal();
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
#pragma endregion

#pragma region Segment Plane
// 線と平面の衝突判定
bool IsSegmentPlaneCollision(const Segment& segment, const Plane& plane) {

	// 垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.GetNormal(), segment.GetDiff());

	// 垂直=平行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}

	// tを求める
	float t = (plane.GetDistance() - Dot(segment.GetOrigin(), plane.GetNormal())) / dot;

	// tの値と線の種類によって衝突しているかを判断する
	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}

	return false;
}
#pragma endregion

#pragma region Triangle Segment
// 三角形と線の衝突判定
bool IsTriangleSegmentCollision(const Triangle& triangle, const Segment& segment) {

	const Vector3* v = triangle.GetVertices();

	// 三角形の頂点
	Vector3 v0 = v[0];
	Vector3 v1 = v[1];
	Vector3 v2 = v[2];

	// 辺ベクトル
	Vector3 edge1 = Subtract(v1, v0);
	Vector3 edge2 = Subtract(v2, v0);

	// 法線
	Vector3 normal = Normalize(Cross(edge1, edge2));

	// 平面との交差チェック
	float denom = Dot(normal, segment.GetDiff());

	// 平行なら交差しない
	if (std::abs(denom) < 1e-6f) {
		return false;
	}

	// tを求める
	float t = Dot(Subtract(v0, segment.GetOrigin()), normal) / denom;

	// 線分範囲外
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	// 交点
	Vector3 p = Add(segment.GetOrigin(), Multiply(t, segment.GetDiff()));

	// 各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を取る
	Vector3 cross01 = Cross(Subtract(v1, v0), Subtract(p, v0));
	Vector3 cross12 = Cross(Subtract(v2, v1), Subtract(p, v1));
	Vector3 cross20 = Cross(Subtract(v0, v2), Subtract(p, v2));

	// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f && Dot(cross20, normal) >= 0.0f) {
		// 街突
		return true;
	}

	return false;
}
#pragma endregion

#pragma region AABB AABB
// AABB同士の衝突判定
bool IsAABBCollision(const AABB& a, const AABB& b) {
	return (
	    (a.GetMin().x <= b.GetMax().x && a.GetMax().x >= b.GetMin().x) &&
		(a.GetMin().y <= b.GetMax().y && a.GetMax().y >= b.GetMin().y) &&
	    (a.GetMin().z <= b.GetMax().z && a.GetMax().z >= b.GetMin().z));
}
#pragma endregion

#pragma region AABB Sphere
// AABBとSphereの衝突判定
bool IsAABBSphereCollision(const AABB& aabb, const Sphere& sphere) {
	// 球
	const Vector3& center = sphere.GetCenter();
	// 最近接点を求める
	Vector3 closestPoint{
		std::clamp(center.x, aabb.GetMin().x, aabb.GetMax().x),
		std::clamp(center.y, aabb.GetMin().y, aabb.GetMax().y),
		std::clamp(center.z, aabb.GetMin().z, aabb.GetMax().z)
	};
	// 最近接点と球の中心との距離を求める
	float distance = Length(closestPoint - sphere.GetCenter());
	// 距離が半径よりも小さければ衝突
	return (distance <= sphere.GetRadius());
}

#pragma endregion

#pragma region AABB Segment

// AABBとSegmentの衝突判定
bool IsAABBSegmentCollision(const AABB& aabb, const Segment& segment) {

	// 線分の始点と終点
	Vector3 p0 = segment.GetOrigin();
	Vector3 d = segment.GetDiff();

	float tmin = 0.0f;
	float tmax = 1.0f;

	Vector3 min = aabb.GetMin();
	Vector3 max = aabb.GetMax();

	// 各軸ごとに処理
	for (int i = 0; i < 3; i++) {

		float start = (&p0.x)[i];
		float dir = (&d.x)[i];

		float minB = (&min.x)[i];
		float maxB = (&max.x)[i];

		if (fabs(dir) < 1e-6f) {

			// 線分がこの軸に平行
			if (start < minB || start > maxB) {
				return false; // AABB外 → 衝突しない
			}
		} else {
			float t1 = (minB - start) / dir;
			float t2 = (maxB - start) / dir;

			// 入れ替え（tNear / tFar）
			float tNear = std::min(t1, t2);
			float tFar = std::max(t1, t2);

			// AABBとの衝突点(貫通点)のtが小さい方
			tmin = std::max(tmin, tNear);
			// AABBとの衝突点(貫通点)のtが大きい方
			tmax = std::min(tmax, tFar);

			if (tmin > tmax) {
				return false; // 交差しない
			}
		}
	}

	// [0,1] 区間で交差していれば線分と衝突
	return true;
}

#pragma endregion