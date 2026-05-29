#include "Clipping.h"
#include "FrustumDebug.h"
#include "Sphere.h"
#include "Matrix4x4.h"
#include <Novice.h>
#include <cmath>

Frustum CreateFrustum(const Matrix4x4& vp) {
	Frustum f;

	// Left
	f.planes[0].normal.x = vp.m[0][3] + vp.m[0][0];
	f.planes[0].normal.y = vp.m[1][3] + vp.m[1][0];
	f.planes[0].normal.z = vp.m[2][3] + vp.m[2][0];
	f.planes[0].distance = vp.m[3][3] + vp.m[3][0];

	// Right
	f.planes[1].normal.x = vp.m[0][3] - vp.m[0][0];
	f.planes[1].normal.y = vp.m[1][3] - vp.m[1][0];
	f.planes[1].normal.z = vp.m[2][3] - vp.m[2][0];
	f.planes[1].distance = vp.m[3][3] - vp.m[3][0];

	// Bottom
	f.planes[2].normal.x = vp.m[0][3] + vp.m[0][1];
	f.planes[2].normal.y = vp.m[1][3] + vp.m[1][1];
	f.planes[2].normal.z = vp.m[2][3] + vp.m[2][1];
	f.planes[2].distance = vp.m[3][3] + vp.m[3][1];

	// Top
	f.planes[3].normal.x = vp.m[0][3] - vp.m[0][1];
	f.planes[3].normal.y = vp.m[1][3] - vp.m[1][1];
	f.planes[3].normal.z = vp.m[2][3] - vp.m[2][1];
	f.planes[3].distance = vp.m[3][3] - vp.m[3][1];

	// Near
	f.planes[4].normal.x = vp.m[0][2];
	f.planes[4].normal.y = vp.m[1][2];
	f.planes[4].normal.z = vp.m[2][2];
	f.planes[4].distance = vp.m[3][2];

	// Far
	f.planes[5].normal.x = vp.m[0][3] - vp.m[0][2];
	f.planes[5].normal.y = vp.m[1][3] - vp.m[1][2];
	f.planes[5].normal.z = vp.m[2][3] - vp.m[2][2];
	f.planes[5].distance = vp.m[3][3] - vp.m[3][2];

	return f;
}

void NormalizePlane(FrustumPlane& p) {
	float len = sqrtf(p.normal.x * p.normal.x + p.normal.y * p.normal.y + p.normal.z * p.normal.z);

	p.normal.x /= len;
	p.normal.y /= len;
	p.normal.z /= len;
	p.distance /= len;
}

bool IsInsideFrustum(const Sphere& sphere, const Frustum& frustum) {

	Vector3 center = sphere.GetCenter();
	float radius = sphere.GetRadius();

	for (int i = 0; i < 6; i++) {

		const auto& p = frustum.planes[i];

		// 球の中心点が平面からどれだけ離れているか
		float distance = Dot(p.normal, center) + p.distance;

		// 判定
		if (distance < -radius) {
			// 完全に外
			return false;
		}
	}
	// 6面で判定して全て内側なら接触中と判断
	return true;
}

// あるカメラの視野を他のカメラから見えるようにする
void DrawFrustum(const Matrix4x4& cameraViewProjection, const Matrix4x4& drawViewProjection, const Matrix4x4& viewportMatrix) {

	// VP行列の逆行列を作る
	Matrix4x4 invVP = Inverse(cameraViewProjection); // ワールド空間での画面上の点を求める

	// NDC空間の8頂点を定義
	Vector3 ndc[8] = {
	    {-1.0f, -1.0f, 0.0f}, // near
	    {-1.0f, 1.0f,  0.0f},
        {1.0f,  1.0f,  0.0f},
        {1.0f,  -1.0f, 0.0f},

	    {-1.0f, -1.0f, 1.0f}, // far
	    {-1.0f, 1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  -1.0f, 1.0f},
	};

	// ワールド空間へ戻す
	Vector3 world[8];

	for (int i = 0; i < 8; i++) {
		world[i] = Transform(ndc[i], invVP);
	}

	// 線を描く						スクリーン座標へ変換して描画
	auto DrawEdge = [&](int a, int b) { DrawClippedLine(world[a], world[b], drawViewProjection, viewportMatrix, 0xFFFF00FF); };

	// Near
	DrawEdge(0, 1);
	DrawEdge(1, 2);
	DrawEdge(2, 3);
	DrawEdge(3, 0);

	// Far
	DrawEdge(4, 5);
	DrawEdge(5, 6);
	DrawEdge(6, 7);
	DrawEdge(7, 4);

	// Side
	DrawEdge(0, 4);
	DrawEdge(1, 5);
	DrawEdge(2, 6);
	DrawEdge(3, 7);
}