#include "Vector3.h"
#include "Matrix4x4.h"
#include <Novice.h>
#define _USE_MATH_DEFINES
#include <assert.h>
#include <cmath>
#include <vector>

//#ifdef ImGui
#include <imgui.h>
#include <string>
//#endif

#pragma region ScreenPrintf関数

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

#pragma endregion

#pragma region Grid

// グリッド表示関数
// Red : z
// Blue : x
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 5.0f;                                      // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                       // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ

	// ビュー射影→ビューポートの合成行列（v * VP * Viewport）
	Matrix4x4 vpvMatrix = Multiply(viewProjectionMatrix, viewportMatrix);

	// 奥から手前への線（X一定でZ方向に伸びる線）
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + kGridEvery * static_cast<float>(xIndex);

		// ワールド座標系上の始点と終点
		Vector3 worldStart{x, 0.0f, -kGridHalfWidth};
		Vector3 worldEnd{x, 0.0f, kGridHalfWidth};

		// スクリーン座標系まで変換
		Vector3 screenStart = Transform(worldStart, vpvMatrix);
		Vector3 screenEnd = Transform(worldEnd, vpvMatrix);

		// 原点を通る線は別の色にする
		unsigned int color = (std::fabs(x) < 1e-4f) ? 0xFF0000FF : 0xAAAAAAFF;

		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);
	}

	// 左右方向の線（Z一定でX方向に伸びる線）
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + kGridEvery * static_cast<float>(zIndex);

		Vector3 worldStart{-kGridHalfWidth, 0.0f, z};
		Vector3 worldEnd{kGridHalfWidth, 0.0f, z};

		Vector3 screenStart = Transform(worldStart, vpvMatrix);
		Vector3 screenEnd = Transform(worldEnd, vpvMatrix);

		unsigned int color = (std::fabs(z) < 1e-4f) ? 0x0000FFFF : 0xAAAAAAFF;

		Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);
	}

	// 原点から上に伸びる高さ方向（Y軸）の線
	// ワールド座標（原点から上方向へ）
	Vector3 worldStartY{0.0f, 0.0f, 0.0f};
	//Vector3 worldStartY{0.0f, -kGridHalfWidth, 0.0f};
	Vector3 worldEndY{0.0f, kGridHalfWidth, 0.0f};

	// スクリーン座標へ変換
	Vector3 screenStart = Transform(worldStartY, vpvMatrix);
	Vector3 screenEnd = Transform(worldEndY, vpvMatrix);

	// 緑色（RGBA）
	unsigned int color = 0x00FF00FF;

	Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);

}

#pragma endregion

#pragma region Sphere

struct Sphere {
	Vector3 center; // 中心点
	float radius;   // 半径
};

// 球を描画する
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;                                 // 分割数
	const float kLonEvery = 2.0f * float(M_PI) / float(kSubdivision); // 経度分割1つ分の角度
	const float kLatEvery = float(M_PI) / float(kSubdivision);        // 緯度分割1つ分の角度
	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // 現在の緯度
		float nextLat = lat + kLatEvery;
		// 経度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度
			float nextLon = lon + kLonEvery;
			// world座標系でのa,b,cを求める
			Vector3 a{sphere.center.x + sphere.radius * cosf(lat) * cosf(lon), sphere.center.y + sphere.radius * sinf(lat), sphere.center.z + sphere.radius * cosf(lat) * sinf(lon)};
			Vector3 b{sphere.center.x + sphere.radius * cosf(nextLat) * cosf(lon), sphere.center.y + sphere.radius * sinf(nextLat), sphere.center.z + sphere.radius * cosf(nextLat) * sinf(lon)};
			Vector3 c{sphere.center.x + sphere.radius * cosf(lat) * cosf(nextLon), sphere.center.y + sphere.radius * sinf(lat), sphere.center.z + sphere.radius * cosf(lat) * sinf(nextLon)};
			// a,b,cをScreen座標系まで変換
			Vector3 aScreen = Transform(a, viewProjectionMatrix);
			aScreen = Transform(aScreen, viewportMatrix);

			Vector3 bScreen = Transform(b, viewProjectionMatrix);
			bScreen = Transform(bScreen, viewportMatrix);

			Vector3 cScreen = Transform(c, viewProjectionMatrix);
			cScreen = Transform(cScreen, viewportMatrix);
			// ab,bcで線を引く
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color);
		}
	}
}

#pragma endregion

// 球同士の当たり判定
bool IsCollision(const Sphere& s1, const Sphere& s2);

// 2点間の距離を求める
double Length(const Vector3& center1, const Vector3& center2) {
	return std::sqrt(
		(center2.x - center1.x) * (center2.x - center1.x) +
		(center2.y - center1.y) * (center2.y - center1.y) +
		(center2.z - center1.z) * (center2.z - center1.z));
}

const char kWindowTitle[] = "Window";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	#pragma region カメラの設定

	// カメラの初期位置, 角度
	Vector3 cameraPosition{0.0f, 0.0f, -20.0f};
	Vector3 cameraRotate{0.5f, 0.0f, 0.0f};
	// カメラ移動速度
	float cameraMoveSpeed = 0.04f;
	float cameraRotateSpeed = 0.01f;

	#pragma endregion

	#pragma region 三角形

	// 三角形のローカル座標
	//Vector3 kTriangleLocalVertices[3]{
	//    {0.0f,  0.8f,  0.0f},
    //    {0.8f,  -0.8f, 0.0f},
    //    {-0.8f, -0.8f, 0.0f}
    //};
	//Vector3 triangleRotate{};
	//Vector3 triangleTranslate{};
	//// スクリーン座標
	//Vector3 screenTriangleVertices[3];

	#pragma endregion

	#pragma region 球

	// 球の初期座標
	std::vector<Vector3> point = {
	    {0.0f, 1.0f, -4.0f},
	    {2.0f, 0.5f, 2.0f},
	};

	// 大きさを決める
	std::vector<Sphere> pointSphere = {
	    {point[0], 1.0f},
	    {point[1], 0.5f}
	};

	float Sphere1MoveSpeed = 0.07f;
	unsigned int sphere1Color = 0xFFFFFFFF;

	#pragma endregion

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		#pragma region ImGui

		//#ifdef ImGui

		ImGui::Begin("window");

		size_t sphereSize = pointSphere.size();
		for (int i = 0; i < sphereSize; ++i) {
			std::string label1 = "Sphere[" + std::to_string(i) + "].center";
			std::string label2 = "Sphere[" + std::to_string(i) + "].Radius";

			ImGui::SliderFloat3(label1.c_str(), &pointSphere[i].center.x, -5.0f, 5.0f);
			ImGui::SliderFloat(label2.c_str(), &pointSphere[i].radius, -0.1f, 5.0f);
		}

		ImGui::End();

		//#endif

		#pragma endregion

		#pragma region カメラの入力処理

		// WASDキーでカメラ移動
		if (keys[DIK_D] != keys[DIK_A]) {
			cameraPosition.x += keys[DIK_D] ? cameraMoveSpeed : -cameraMoveSpeed;
		}
		if (keys[DIK_W] != keys[DIK_S]) {
			cameraPosition.y += keys[DIK_W] ? cameraMoveSpeed : -cameraMoveSpeed;
		}
		if (keys[DIK_E] != keys[DIK_Q]) {
			cameraPosition.z += keys[DIK_E] ? cameraMoveSpeed * 2.0f : -cameraMoveSpeed * 2.0f;
		}

		// カメラの回転
		if (keys[DIK_U] != keys[DIK_J]) {
			cameraRotate.x += keys[DIK_U] ? cameraRotateSpeed : -cameraRotateSpeed;
		}
		if (keys[DIK_K] != keys[DIK_H]) {
			cameraRotate.y += keys[DIK_K] ? cameraRotateSpeed : -cameraRotateSpeed;
		}
		if (keys[DIK_I] != keys[DIK_Y]) {
			cameraRotate.z += keys[DIK_I] ? cameraRotateSpeed : -cameraRotateSpeed;
		}

		#pragma endregion

		#pragma region 球の入力処理

		// 上下キーで球1の前後移動
		if (!keys[DIK_LSHIFT] && (keys[DIK_UP] != keys[DIK_DOWN])) {
			pointSphere[0].center.z += keys[DIK_UP] ? Sphere1MoveSpeed : -Sphere1MoveSpeed;
		}

		// 左右キーで球1の左右移動
		if (keys[DIK_RIGHT] != keys[DIK_LEFT]) {
			pointSphere[0].center.x += keys[DIK_RIGHT] ? Sphere1MoveSpeed : -Sphere1MoveSpeed;
		}

		// 左SHIFT + 上下キーで球1の高さ移動
		if (keys[DIK_LSHIFT] && (keys[DIK_UP] != keys[DIK_DOWN])) {
			pointSphere[0].center.y += keys[DIK_UP] ? Sphere1MoveSpeed : -Sphere1MoveSpeed;
		}

		#pragma endregion

		// 三角形のY軸回転
		// rotate.y += 0.02f;

		// 2つの球の中心点間の距離を求める
		double distance = Length(pointSphere[0].center, pointSphere[1].center);

		if (distance <= pointSphere[0].radius + pointSphere[1].radius) {
			sphere1Color = 0xFF0000FF;
		} else {
			sphere1Color = 0xFFFFFFFF;
		}

		#pragma region カメラのスケール・回転・平行移動

		// カメラの回転行列（逆回転）
		Matrix4x4 cameraRotX = MakeRotateXMatrix(-cameraRotate.x);
		Matrix4x4 cameraRotY = MakeRotateYMatrix(-cameraRotate.y);
		Matrix4x4 cameraRotZ = MakeRotateZMatrix(-cameraRotate.z);

		// カメラの平行移動（逆方向）
		Matrix4x4 cameraTrans = MakeTranslateMatrix({-cameraPosition.x, -cameraPosition.y, -cameraPosition.z});

		#pragma endregion

		#pragma region 行列計算

		// ビュー行列 = R^-1 * T^-1
		Matrix4x4 viewMatrix = Multiply(Multiply(cameraRotX, cameraRotY), Multiply(cameraRotZ, cameraTrans));

		// 各種行列の計算
		// 行列の計算・変換は描画の直前に置くのが良い
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		#pragma endregion

		#pragma region スクリーン座標に変換

		// 三角形のSRTをMatrixに変換
		//Matrix4x4 worldMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, triangleRotate, triangleTranslate);
		//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		//// 頂点を変換
		//for (uint32_t i = 0; i < 3; ++i) {
		//	Vector3 ndVertex = Transform(kTriangleLocalVertices[i], worldViewProjectionMatrix);
		//	screenTriangleVertices[i] = Transform(ndVertex, viewportMatrix);
		//}

		// 線分の始点と終点を変換
		//Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
		//Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

		#pragma endregion

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		#pragma region Draw

		// グリッド
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 三角形
		//Novice::DrawTriangle(
		//    int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);

		// 点の描画
		DrawSphere(pointSphere[1], viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);
		DrawSphere(pointSphere[0], viewProjectionMatrix, viewportMatrix, sphere1Color);

		// 線分
		//Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), 0xFFFFFFFF);

		#pragma endregion

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
