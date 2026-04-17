#include "Vector3.h"
#include "Matrix4x4.h"
#include <Novice.h>
#define _USE_MATH_DEFINES
#include <assert.h>
#include <cmath>

#ifdef ImGui
#include <imgui.h>
#endif

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
	const float kGridHalfWidth = 2.0f;                                      // Gridの半分の幅
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
	Vector3 cameraPosition{0.0f, 0.0f, -6.0f};
	Vector3 cameraRotate{0.3f, 0.0f, 0.0f};
	// カメラ移動速度
	//float moveSpeed = 0.1f;
	//float rotateSpeed = 0.02f;

	#pragma endregion

	#pragma region 三角形

	// 三角形のローカル座標
	Vector3 kTriangleLocalVertices[3]{
	    {0.0f,  0.8f,  0.0f},
        {0.8f,  -0.8f, 0.0f},
        {-0.8f, -0.8f, 0.0f}
    };
	Vector3 triangleRotate{};
	Vector3 triangleTranslate{};
	// スクリーン座標
	Vector3 screenTriangleVertices[3];

	#pragma endregion

	#pragma region 点と線分

	Segment segment{
	    {-2.0f, -1.0f, 0.0f},
        {3.0f,  2.0f,  2.0f}
    };
	Vector3 point{-1.5f, 0.6f, 0.6f};
	// pointを線分に射影したベクトル。今回は正しく計算できているかを確認するためだけに使う
	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);

	// この値が線分上の点を表す
	Vector3 closestPoint = ClosestPoint(point, segment);

	Sphere pointSphere{point, 0.01f};
	Sphere closestPointSphere{closestPoint, 0.01f};

	#pragma endregion

	//Vector3 v1{1.2f, -3.9f, 2.5f};
	//Vector3 v2{2.8f, 0.4f, -1.3f};

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

		#ifdef ImGui

		ImGui::Begin("window");
		// ImGui::DragFloat3("CameraTranlate", &cameraPosition.x, 0.01f);
		// ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		// ImGui::DragFloat3("SphereCenter", &pointSphere.center.x, 0.01f);
		// ImGui::DragFloat3("SphereRadius", &pointSphere.radius, 0.01f);

		ImGui::InputFloat3("Point", &point.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Segment origin", &segment.origin.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Segment diff", &segment.diff.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::End();

		#endif

		#pragma endregion

		#pragma region カメラの入力処理

		// 上下左右キーでカメラ移動
		//if (keys[DIK_RIGHT] != keys[DIK_LEFT])
		//	cameraPosition.x += keys[DIK_RIGHT] ? moveSpeed : -moveSpeed;
		//if (keys[DIK_UP] != keys[DIK_DOWN])
		//	cameraPosition.y += keys[DIK_UP] ? moveSpeed : -moveSpeed;
		//if (keys[DIK_O] != keys[DIK_L])
		//	cameraPosition.z += keys[DIK_O] ? moveSpeed : -moveSpeed;

		// カメラの回転（ピッチ角変更）
		//if (keys[DIK_U] != keys[DIK_J])
		//	cameraRotate.x += keys[DIK_U] ? rotateSpeed : -rotateSpeed;
		//if (keys[DIK_K] != keys[DIK_H])
		//	cameraRotate.y += keys[DIK_K] ? rotateSpeed : -rotateSpeed;

		#pragma endregion

		#pragma region 球の入力処理

		// WSキーで球の前後移動
		//if (keys[DIK_W] != keys[DIK_S])
		//	pointSphere.center.z += keys[DIK_W] ? 0.1f : -0.1f;

		// ADキーで球の左右移動
		//if (keys[DIK_D] != keys[DIK_A])
		//	pointSphere.center.x += keys[DIK_D] ? 0.05f : -0.05f;

		#pragma endregion

		// 三角形のY軸回転
		// rotate.y += 0.02f;

		//Vector3 cross = Cross(v1, v2);

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
		Matrix4x4 worldMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, triangleRotate, triangleTranslate);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		// 頂点を変換
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndVertex = Transform(kTriangleLocalVertices[i], worldViewProjectionMatrix);
			screenTriangleVertices[i] = Transform(ndVertex, viewportMatrix);
		}

		// 線分の始点と終点を変換
		Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

		#pragma endregion

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//VectorScreenPrintf(0, 0, cross, "Cross");

		#pragma region Draw

		// グリッド
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 三角形
		//Novice::DrawTriangle(
		//    int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);

		// 点の描画
		DrawSphere(pointSphere, viewProjectionMatrix, viewportMatrix, 0xFF0000FF);
		DrawSphere(closestPointSphere, viewProjectionMatrix, viewportMatrix, 0x000000FF);

		// 線分
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), 0xFFFFFFFF);

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
