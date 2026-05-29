#pragma once
struct Vector3 {
	float x, y, z;
	Vector3 operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
	Vector3 operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
	Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
	float& operator[](int i) { return *(&x + i); }
	const float& operator[](int i) const { return *(&x + i); }
};