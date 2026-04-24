#pragma once

// 2点間の距離を求める
float Length(const Vector3& center1, const Vector3& center2);

// 球同士の当たり判定
bool IsSphereSphereCollision(const Sphere& s1, const Sphere& s2);

// 球と平面の衝突判定
bool IsSpherePlaneCollision(const Sphere& sphere, const Plane& plane);