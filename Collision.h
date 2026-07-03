#pragma once

struct Vector3;
class Sphere;
class Plane;
class Segment;
class Triangle;
class AABB;
class OBB;

// 2点間の距離を求める
float Length(const Vector3& center1, const Vector3& center2);

// 球同士の当たり判定
bool IsSphereSphereCollision(const Sphere& s1, const Sphere& s2);

// 球と平面の衝突判定
bool IsSpherePlaneCollision(const Sphere& sphere, const Plane& plane);

// カプセル(startからendへスイープした球)と平面の衝突判定
bool IsCapsulePlaneCollision(const Vector3& start, const Vector3& end, float radius, const Plane& plane);

// 線と平面の衝突判定
bool IsSegmentPlaneCollision(const Segment& segment, const Plane& plane);

// 三角形と線の衝突判定
bool IsTriangleSegmentCollision(const Triangle& triangle, const Segment& segment);

// AABB衝突判定
bool IsAABBCollision(const AABB& a, const AABB& b);

// AABBとSphereの衝突判定
bool IsAABBSphereCollision(const AABB& aabb, const Sphere& sphere);

// AABBとSegmentの衝突判定
bool IsAABBSegmentCollision(const AABB& aabb, const Segment& segment);

// OBBと球の衝突判定
bool IsOBBSphereCollision(const OBB& obb, const Sphere& sphere);

// OBBとSegmentの衝突判定
bool IsOBBSegmentCollision(const OBB& obb, const Segment& segment);

// OBB同士の衝突判定
bool IsOBBCollision(const OBB& a, const OBB& b);

// OBBとAABBの衝突判定
bool IsOBBAABBCollision(const OBB& obb, const AABB& aabb);