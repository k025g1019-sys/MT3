#include "Collision.h"
#include "AABB.h"
#include "OBB.h"
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

#pragma region Capsule Plane
// カプセル(startからendへスイープした球)と平面の衝突判定
bool IsCapsulePlaneCollision(const Vector3& start, const Vector3& end, float radius, const Plane& plane) {
	const Vector3& normal = plane.GetNormal();
	float d = plane.GetDistance();

	// 両端点の平面までの符号付き距離
	float startDistance = Dot(start, normal) - d;
	float endDistance = Dot(end, normal) - d;

	// 符号が異なれば線分が平面を貫いている
	if (startDistance * endDistance <= 0.0f) {
		return true;
	}

	// 貫いていなければ、平面に近い方の端点との距離で判定
	return std::min(std::abs(startDistance), std::abs(endDistance)) <= radius;
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

#pragma region OBB Sphere

bool IsOBBSphereCollision(const OBB& obb, const Sphere& sphere) {
	const Vector3& c = sphere.GetCenter();

	// OBB中心 → 球中心ベクトル
	Vector3 d = Subtract(c, obb.GetCenter());

	Vector3 closest = obb.GetCenter();

	const auto& axis = obb.GetOrientations();
	const Vector3& half = obb.GetSize();

	// 各ローカル軸方向に射影してクランプ
	for (int i = 0; i < 3; i++) {

		float dist = Dot(d, axis[i]); // 軸方向成分

		// 半サイズで制限
		if (dist > half[i])
			dist = half[i];
		if (dist < -half[i])
			dist = -half[i];

		closest = Add(closest, Multiply(dist, axis[i]));
	}

	// 最近接点 → 球中心の距離
	Vector3 diff = Subtract(c, closest);

	float distSq = Dot(diff, diff);

	float r = sphere.GetRadius();

	return distSq <= r * r;
}

#pragma endregion

#pragma region OBB Segment

// OBBとSegmentの衝突判定
bool IsOBBSegmentCollision(const OBB& obb, const Segment& segment) {

	// OBB情報
	const Vector3& center = obb.GetCenter();
	const auto& axis = obb.GetOrientations();
	const Vector3& halfSize = obb.GetSize();

	// Segment
	Vector3 p0 = segment.GetOrigin();
	Vector3 p1 = Add(segment.GetOrigin(), segment.GetDiff());

	// OBB中心基準へ移動
	Vector3 localP0 = Subtract(p0, center);
	Vector3 localP1 = Subtract(p1, center);

	// OBBローカル空間へ変換
	Vector3 p0Local{Dot(localP0, axis[0]), Dot(localP0, axis[1]), Dot(localP0, axis[2])};

	Vector3 p1Local{Dot(localP1, axis[0]), Dot(localP1, axis[1]), Dot(localP1, axis[2])};

	// ローカル空間でのSegment
	Segment localSegment;
	localSegment.SetOrigin(p0Local);
	localSegment.SetDiff(Subtract(p1Local, p0Local));

	// OBB → AABB化
	AABB localAABB;
	localAABB.SetMin(Vector3{-halfSize.x, -halfSize.y, -halfSize.z});

	localAABB.SetMax(Vector3{halfSize.x, halfSize.y, halfSize.z});

	// AABB vs Segment 判定
	return IsAABBSegmentCollision(localAABB, localSegment);
}

#pragma endregion

#pragma region OBB OBB

bool IsOBBCollision(const OBB& a, const OBB& b) {

	const Vector3& centerA = a.GetCenter();
	const Vector3& centerB = b.GetCenter();

	const auto& axisA = a.GetOrientations();
	const auto& axisB = b.GetOrientations();

	const Vector3& halfA = a.GetSize();
	const Vector3& halfB = b.GetSize();

	constexpr float EPSILON = 1e-6f;

	// 中心間ベクトル
	Vector3 tWorld = Subtract(centerB, centerA);

	// A基準へ変換
	float t[3] = {Dot(tWorld, axisA[0]), Dot(tWorld, axisA[1]), Dot(tWorld, axisA[2])};

	// 回転行列
	float R[3][3];
	float AbsR[3][3];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			R[i][j] = Dot(axisA[i], axisB[j]);
			AbsR[i][j] = std::fabs(R[i][j]) + EPSILON;
		}
	}

	float ra, rb;

	// =====================
	// Aの軸 3本
	// =====================
	for (int i = 0; i < 3; i++) {

		ra = halfA[i];

		rb = halfB.x * AbsR[i][0] + halfB.y * AbsR[i][1] + halfB.z * AbsR[i][2];

		if (std::fabs(t[i]) > ra + rb) {
			return false;
		}
	}

	// =====================
	// Bの軸 3本
	// =====================
	for (int j = 0; j < 3; j++) {

		ra = halfA.x * AbsR[0][j] + halfA.y * AbsR[1][j] + halfA.z * AbsR[2][j];

		rb = halfB[j];

		float distance = std::fabs(t[0] * R[0][j] + t[1] * R[1][j] + t[2] * R[2][j]);

		if (distance > ra + rb) {
			return false;
		}
	}

	// =====================
	// 外積軸 9本
	// =====================

	// A0 x B0
	ra = halfA.y * AbsR[2][0] + halfA.z * AbsR[1][0];
	rb = halfB.y * AbsR[0][2] + halfB.z * AbsR[0][1];
	if (std::fabs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)
		return false;

	// A0 x B1
	ra = halfA.y * AbsR[2][1] + halfA.z * AbsR[1][1];
	rb = halfB.x * AbsR[0][2] + halfB.z * AbsR[0][0];
	if (std::fabs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)
		return false;

	// A0 x B2
	ra = halfA.y * AbsR[2][2] + halfA.z * AbsR[1][2];
	rb = halfB.x * AbsR[0][1] + halfB.y * AbsR[0][0];
	if (std::fabs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)
		return false;

	// A1 x B0
	ra = halfA.x * AbsR[2][0] + halfA.z * AbsR[0][0];
	rb = halfB.y * AbsR[1][2] + halfB.z * AbsR[1][1];
	if (std::fabs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)
		return false;

	// A1 x B1
	ra = halfA.x * AbsR[2][1] + halfA.z * AbsR[0][1];
	rb = halfB.x * AbsR[1][2] + halfB.z * AbsR[1][0];
	if (std::fabs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)
		return false;

	// A1 x B2
	ra = halfA.x * AbsR[2][2] + halfA.z * AbsR[0][2];
	rb = halfB.x * AbsR[1][1] + halfB.y * AbsR[1][0];
	if (std::fabs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)
		return false;

	// A2 x B0
	ra = halfA.x * AbsR[1][0] + halfA.y * AbsR[0][0];
	rb = halfB.y * AbsR[2][2] + halfB.z * AbsR[2][1];
	if (std::fabs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)
		return false;

	// A2 x B1
	ra = halfA.x * AbsR[1][1] + halfA.y * AbsR[0][1];
	rb = halfB.x * AbsR[2][2] + halfB.z * AbsR[2][0];
	if (std::fabs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)
		return false;

	// A2 x B2
	ra = halfA.x * AbsR[1][2] + halfA.y * AbsR[0][2];
	rb = halfB.x * AbsR[2][1] + halfB.y * AbsR[2][0];
	if (std::fabs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)
		return false;

	return true;
}

#pragma endregion

#pragma region OBB AABB

OBB ConvertAABBToOBB(const AABB& aabb) {
	OBB result;

	result.SetCenter({(aabb.GetMin().x + aabb.GetMax().x) * 0.5f, (aabb.GetMin().y + aabb.GetMax().y) * 0.5f, (aabb.GetMin().z + aabb.GetMax().z) * 0.5f});

	result.SetSize({(aabb.GetMax().x - aabb.GetMin().x) * 0.5f, (aabb.GetMax().y - aabb.GetMin().y) * 0.5f, (aabb.GetMax().z - aabb.GetMin().z) * 0.5f});

	Vector3 axis[3] = {
	    {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

	for (int i = 0; i < 3; ++i) {
		result.SetOrientation(i, axis[i]);
	}

	return result;
}

bool IsOBBAABBCollision(const OBB& obb, const AABB& aabb) { return IsOBBCollision(obb, ConvertAABBToOBB(aabb)); }

#pragma endregion