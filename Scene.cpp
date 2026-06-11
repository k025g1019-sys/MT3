#include "Scene.h"
#include "DebugGrid.h"
#include <Novice.h>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region SceneManagerB

SceneManager::~SceneManager() {}

void SceneManager::SetScene(std::unique_ptr<Scene> scene) { next = std::move(scene); }

void SceneManager::Update() {
	if (next) {
		current = std::move(next);
	}
	if (current) {
		current->Update(*this);
	}
}

void SceneManager::Draw() {
	if (current) {
		current->Draw();
	}
}

#pragma endregion

#pragma region TitleScene

TitleScene::TitleScene() {}

#pragma region Update
void TitleScene::Update(SceneManager& manager) {
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

	if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
		manager.SetScene(std::make_unique<GameScene>());
	}
}
#pragma endregion

#pragma region Draw
void TitleScene::Draw() {}
#pragma endregion

#pragma endregion

#pragma region GameScene

#pragma region Initialize
GameScene::GameScene() {
	objects = new Objects;
	viewProjectionMatrix = MakePerspectiveFovMatrix(0.50f, 1280.0f / 720.0f, 0.1f, 2000.0f);
	viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 2000.0f);
}
#pragma endregion

#pragma region Update
void GameScene::Update(SceneManager& manager) {

	// キー入力を受け取る
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

	if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
		manager.SetScene(std::make_unique<TitleScene>());
		return;
	}

#pragma region 入力処理

#pragma endregion

#pragma region 衝突判定

	objects->UpdateAllCollisions();

#pragma endregion

	camera.Update(kWindowWidth, kWindowHeight, viewProjectionMatrix, viewportMatrix, keys);
}
#pragma endregion

#pragma region Draw
// 描画
void GameScene::Draw() {
	DrawGrid(viewProjectionMatrix, viewportMatrix);
	objects->Draw(viewProjectionMatrix, viewportMatrix);
	
#ifdef _DEBUG
#pragma region ImGui

	objects->DrawImgui();

#pragma region Camera
	ImGui::Begin("Camera");

	Vector3 pos = camera.GetPosition();
	Vector3 rot = camera.GetRotation();

	ImGui::DragFloat3("Position", &pos.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rot.x, 0.01f);

	camera.SetPosition(pos);
	camera.SetRotation(rot);

	ImGui::End();
#pragma endregion

#endif

#pragma endregion
}

#pragma endregion

#pragma endregion
