#include "Spherical.h"
#include "Vector3.h"
#include <cmath>
#include <algorithm>

Vector3 ToCartesian(const Spherical& s) {
	// 仰角θで、高さ(y)と水平面に落とした影の長さ(rho)に分ける
	float rho = s.radius * std::cos(s.theta);
	// 方位角φで、影の長さrhoをxとzに分ける
	return {rho * std::cos(s.phi), s.radius * std::sin(s.theta), rho * std::sin(s.phi)};
}

Spherical ToSpherical(const Vector3& p) {
	float r = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	if (r == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}
	float sinTheta = std::clamp(p.y / r, -1.0f, 1.0f);
	float phi = 0.0f;
	if (p.x != 0.0f || p.z != 0.0f) {
		phi = std::atan2(p.z, p.x);
	}
	return {r, std::asin(sinTheta), phi};
}