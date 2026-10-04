#pragma once

struct Vector3;

/// <summary>
/// 球面座標。原点からの「距離」と「2つの角度」で位置を表す
/// </summary>
struct Spherical {
	float radius; // 動径 r : 原点からの距離
	float theta;  // 仰角 θ : 水平面(XZ平面)から上下にどれだけ傾けたか。+で上
	float phi;    // 方位角 φ : 水平面上で+X軸からどれだけ回したか。+X→+Zの向きが+
};

// 球面座標 → 直交座標
Vector3 ToCartesian(const Spherical& s);

// 直交座標 → 球面座標
Spherical ToSpherical(const Vector3& p);