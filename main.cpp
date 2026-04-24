#include <Novice.h>
#include "Scene.h"

const char kWindowTitle[] = "Window";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	SceneManager manager;
	manager.SetScene(new GameScene());

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	#pragma region Triangle

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

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		#pragma region Triangle

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

		manager.Update();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		manager.Draw();

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
