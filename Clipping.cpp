#include "Clipping.h"
#include <Novice.h>

static Vector4 Lerp(const Vector4& a, const Vector4& b, float t) {
	return {
	    a.x + (b.x - a.x) * t,
	    a.y + (b.y - a.y) * t,
	    a.z + (b.z - a.z) * t,
	    a.w + (b.w - a.w) * t,
	};
}

Vector4 TransformToClip(const Vector3& v, const Matrix4x4& m) {

	Vector4 result;

	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];

	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];

	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];

	result.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	return result;
}

static bool ClipPlane(float da, float db, Vector4& a, Vector4& b) {

	// 完全に外
	if (da < 0.0f && db < 0.0f) {
		return false;
	}

	// 完全に内
	if (da >= 0.0f && db >= 0.0f) {
		return true;
	}

	// 交点計算
	float t = da / (da - db);

	Vector4 p = Lerp(a, b, t);

	if (da < 0.0f) {
		a = p;
	} else {
		b = p;
	}

	return true;
}

bool ClipLine(Vector4& a, Vector4& b) {

	// Left
	if (!ClipPlane(a.x + a.w, b.x + b.w, a, b)) {
		return false;
	}

	// Right
	if (!ClipPlane(-a.x + a.w, -b.x + b.w, a, b)) {
		return false;
	}

	// Bottom
	if (!ClipPlane(a.y + a.w, b.y + b.w, a, b)) {
		return false;
	}

	// Top
	if (!ClipPlane(-a.y + a.w, -b.y + b.w, a, b)) {
		return false;
	}

	// Near (DirectX)
	if (!ClipPlane(a.z, b.z, a, b)) {
		return false;
	}

	// Far
	if (!ClipPlane(-a.z + a.w, -b.z + b.w, a, b)) {
		return false;
	}

	return true;
}

bool DrawClippedLine(const Vector3& start, const Vector3& end, const Matrix4x4& viewProjection, const Matrix4x4& viewport, unsigned int color) {

	// 3D座標からクリップ空間へ変換
	Vector4 clip0 = TransformToClip(start, viewProjection);
	Vector4 clip1 = TransformToClip(end, viewProjection);

	// クリッピングを行う
	if (!ClipLine(clip0, clip1)) {
		return false;
	}

	// NDC変換
	Vector3 ndc0 = {
	    clip0.x / clip0.w,
	    clip0.y / clip0.w,
	    clip0.z / clip0.w,
	};

	Vector3 ndc1 = {
	    clip1.x / clip1.w,
	    clip1.y / clip1.w,
	    clip1.z / clip1.w,
	};

	// スクリーン座標へ変換
	Vector3 screen0 = Transform(ndc0, viewport);
	Vector3 screen1 = Transform(ndc1, viewport);

	Novice::DrawLine(int(screen0.x), int(screen0.y), int(screen1.x), int(screen1.y), color);

	return true;
}