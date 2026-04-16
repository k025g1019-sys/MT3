#include <Novice.h>
#define _USE_MATH_DEFINES
#include <assert.h>
#include <cmath>
#include <utility>
// #ifdef ImGui
#include <imgui.h>
// #endif

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
};

#pragma region 関数宣言

#pragma region

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
// 単位行列の作成
Matrix4x4 MakeIdentity4x4();

#pragma endregion

#pragma region

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

#pragma endregion

#pragma region

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float faeClip);
// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
// ビューポート行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);

#pragma endregion

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

const char kWindowTitle[] = "LE2A_12_スズキ_ダイスケ_MT3_01_02";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// カメラの初期位置, 角度
	Vector3 cameraPosition{0.0f, 0.0f, -6.0f};
	Vector3 cameraRotate{0.3f, 0.0f, 0.0f};

	// カメラ移動速度
	float moveSpeed = 0.1f;
	float rotateSpeed = 0.02f;

	// 三角形のローカル座標
	Vector3 kLocalVertices[3]{
	    {0.0f,  0.8f,  0.0f},
        {0.8f,  -0.8f, 0.0f},
        {-0.8f, -0.8f, 0.0f}
    };

	Vector3 rotate{};
	Vector3 translate{};

	Vector3 screenVertices[3];

	Sphere sphere{
	    {0.0f, 1.0f, 0.0f},
        1.0f
    };

	Vector3 v1{1.2f, -3.9f, 2.5f};
	Vector3 v2{2.8f, 0.4f, -1.3f};

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

		// 上下左右キーでカメラ移動
		if (keys[DIK_RIGHT] != keys[DIK_LEFT])
			cameraPosition.x += keys[DIK_RIGHT] ? moveSpeed : -moveSpeed;
		if (keys[DIK_UP] != keys[DIK_DOWN])
			cameraPosition.y += keys[DIK_UP] ? moveSpeed : -moveSpeed;
		if (keys[DIK_O] != keys[DIK_L])
			cameraPosition.z += keys[DIK_O] ? moveSpeed : -moveSpeed;

		// カメラの回転（ピッチ角変更）
		if (keys[DIK_U] != keys[DIK_J])
			cameraRotate.x += keys[DIK_U] ? rotateSpeed : -rotateSpeed;
		if (keys[DIK_K] != keys[DIK_H])
			cameraRotate.y += keys[DIK_K] ? rotateSpeed : -rotateSpeed;

		// 三角形のY軸回転
		rotate.y += 0.02f;

		// WSキーで前後移動
		if (keys[DIK_W] != keys[DIK_S])
			sphere.center.z += keys[DIK_W] ? 0.1f : -0.1f;

		// ADキーで左右移動
		if (keys[DIK_D] != keys[DIK_A])
			sphere.center.x += keys[DIK_D] ? 0.05f : -0.05f;

		// #ifdef ImGui

		ImGui::Begin("window");
		ImGui::DragFloat3("CameraTranlate", &cameraPosition.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &sphere.center.x, 0.01f);
		ImGui::DragFloat3("SphereRadius", &sphere.radius, 0.01f);
		ImGui::End();

		// #endif

		Vector3 cross = Cross(v1, v2);

		// カメラの回転行列（逆回転）
		Matrix4x4 cameraRotX = MakeRotateXMatrix(-cameraRotate.x);
		Matrix4x4 cameraRotY = MakeRotateYMatrix(-cameraRotate.y);
		Matrix4x4 cameraRotZ = MakeRotateZMatrix(-cameraRotate.z);

		// カメラの平行移動（逆方向）
		Matrix4x4 cameraTrans = MakeTranslateMatrix({-cameraPosition.x, -cameraPosition.y, -cameraPosition.z});

		// ビュー行列 = R^-1 * T^-1
		Matrix4x4 viewMatrix = Multiply(Multiply(cameraRotX, cameraRotY), Multiply(cameraRotZ, cameraTrans));

		// 各種行列の計算
		Matrix4x4 worldMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translate);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, cross, "Cross");

		DrawGrid(Multiply(viewMatrix, projectionMatrix), viewportMatrix);

		// 描画
		//Novice::DrawTriangle(
		//    int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);

		DrawSphere(sphere, Multiply(viewMatrix, projectionMatrix), viewportMatrix, 0xFFFFFFFF);

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

#pragma region 関数定義

#pragma region

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
};

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
};

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
};

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 a = m;                   // 作業用
	Matrix4x4 inv = MakeIdentity4x4(); // 単位行列

	for (int i = 0; i < 4; i++) {
		// ピボット選択（0 なら失敗）
		float pivot = a.m[i][i];
		if (fabs(pivot) < 1e-6f) {
			// ピボットが小さすぎる場合、行を交換する
			for (int r = i + 1; r < 4; r++) {
				if (fabs(a.m[r][i]) > 1e-6f) {
					// std::swap ... 変数同士の値を入れ替える
					std::swap(a.m[i], a.m[r]);
					std::swap(inv.m[i], inv.m[r]);
					pivot = a.m[i][i];
					break;
				}
			}
		}

		// それでも pivot が 0 なら逆行列なし
		if (fabs(pivot) < 1e-6f) {
			return MakeIdentity4x4(); // 失敗時の代替（適宜変更）
		}

		// ピボット行を 1 に正規化
		float invPivot = 1.0f / pivot;
		for (int j = 0; j < 4; j++) {
			a.m[i][j] *= invPivot;
			inv.m[i][j] *= invPivot;
		}

		// 他の行からピボット列を消去
		for (int r = 0; r < 4; r++) {
			if (r == i)
				continue;
			float factor = a.m[r][i];
			for (int c = 0; c < 4; c++) {
				a.m[r][c] -= factor * a.m[i][c];
				inv.m[r][c] -= factor * inv.m[i][c];
			}
		}
	}

	return inv;
}

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
};

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}
	return result;
}

#pragma endregion

#pragma region

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}
// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}
// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();
	float c = std::cos(radian);
	float s = std::sin(radian);

	result.m[1][1] = c;
	result.m[1][2] = s;
	result.m[2][1] = -s;
	result.m[2][2] = c;

	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();
	float c = std::cos(radian);
	float s = std::sin(radian);

	result.m[0][0] = c;
	result.m[0][2] = -s;
	result.m[2][0] = s;
	result.m[2][2] = c;

	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();
	float c = std::cos(radian);
	float s = std::sin(radian);

	result.m[0][0] = c;
	result.m[0][1] = s;
	result.m[1][0] = -s;
	result.m[1][1] = c;

	return result;
}

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	// 各行列を作成
	Matrix4x4 scaleMat = MakeScaleMatrix(scale);
	Matrix4x4 rotXMat = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotYMat = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotZMat = MakeRotateZMatrix(rotate.z);
	Matrix4x4 transMat = MakeTranslateMatrix(translate);

	// 回転行列を合成（X → Y → Z）
	Matrix4x4 rotMat = Multiply(rotXMat, Multiply(rotYMat, rotZMat));

	// アフィン行列 = S * R * T
	Matrix4x4 affine = Multiply(scaleMat, Multiply(rotMat, transMat));

	return affine;
}

#pragma endregion

#pragma region

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};

	float yScale = 1.0f / std::tan(fovY * 0.5f);
	float xScale = yScale / aspectRatio;

	result.m[0][0] = xScale;
	result.m[1][1] = yScale;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
}

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);

	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;

	return result;
}

// ビューポート行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;

	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;

	return result;
}

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

#pragma endregion

#pragma endregion
