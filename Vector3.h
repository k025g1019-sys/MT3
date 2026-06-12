#pragma once

struct Vector3 {
	float x, y, z;

	// 加算代入
	Vector3& operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	// 減算代入
	Vector3& operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	// スカラー倍代入
	Vector3& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	// スカラー除算代入
	Vector3& operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	float& operator[](int i) { return *(&x + i); }
	const float& operator[](int i) const { return *(&x + i); }
};

// 加算
inline Vector3 operator+(const Vector3& v1, const Vector3& v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }

// 減算
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }

// ベクトル * スカラー
inline Vector3 operator*(const Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }

// スカラー * ベクトル
inline Vector3 operator*(float s, const Vector3& v) { return {v.x * s, v.y * s, v.z * s}; }

// ベクトル / スカラー
inline Vector3 operator/(const Vector3& v, float s) { return {v.x / s, v.y / s, v.z / s}; }


inline Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
inline Vector3 operator+(const Vector3& v) { return v; }