#include "Scene.h"
#include "Collision.h"
#include <Novice.h>
// #ifdef ImGui
#include <imgui.h>
#include <string>
// #endif

#pragma region SceneManager

SceneManager::~SceneManager() { delete current; }

void SceneManager::SetScene(Scene* scene) { next = scene; }

void SceneManager::Update() {
	if (next) {
		delete current;
		current = next;
		next = nullptr;
	}
	if (current)
		current->Update(*this);
}

void SceneManager::Draw() {
	if (current)
		current->Draw();
}

#pragma endregion

#pragma region TitleScene

TitleScene::TitleScene() {}

void TitleScene::Update(SceneManager& manager) {
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

	if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
		manager.SetScene(new GameScene());
	}
}

void TitleScene::Draw() {}

#pragma endregion

#pragma region GameScene

GameScene::GameScene() {

	spheres = {
	    Sphere({.center = {0.0f, 0.5f, -2.0f}, .radius = 0.5f, .moveSpeed = 0.03f}
        ),
    };

	planes = {
	    Plane({.normal = {0.0f, 1.0f, 0.0f}, .distance = 1.5f}
		),
	};

	viewProjectionMatrix = MakePerspectiveFovMatrix(0.50f, 1280.0f / 720.0f, 0.1f, 2000.0f);
	viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 2000.0f);
}

void GameScene::Update(SceneManager& manager) {

	// キー入力を受け取る
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

	#pragma region ImGui

	// #ifdef ImGui
	ImGui::Begin("window");
	size_t sphereSize = spheres.size();
	for (size_t i = 0; i < sphereSize; ++i) {
		std::string label1 = "Sphere[" + std::to_string(i) + "].center";
		std::string label2 = "Sphere[" + std::to_string(i) + "].Radius";

		Vector3 sphereCenter = spheres[i].GetCenter();
		float sphereRadius = spheres[i].GetRadius();
		ImGui::DragFloat3(label1.c_str(), &sphereCenter.x, 0.01f);
		ImGui::DragFloat(label2.c_str(), &sphereRadius, 0.01f);
		spheres[i].SetCenter(sphereCenter);
		spheres[i].SetRadius(sphereRadius);
	}
	size_t planeSize = planes.size();
	for (size_t i = 0; i < planeSize; ++i) {
		std::string label1 = "Plane[" + std::to_string(i) + "].Normal";
		std::string label2 = "Plane[" + std::to_string(i) + "].Distance";

		Vector3 planeNormal = planes[i].GetNormal();
		float planeDistance = planes[i].GetDistance();
		ImGui::DragFloat3(label1.c_str(), &planeNormal.x, 0.01f);
		ImGui::DragFloat(label2.c_str(), &planeDistance, 0.01f);
		planeNormal = Normalize(planeNormal);
		planes[i].SetNormal(planeNormal);
		planes[i].SetDistance(planeDistance);
	}
	ImGui::End();

	ImGui::Begin("CameraTranslate");
	Vector3 cameraPosition_ = camera.GetPosition();
	Vector3 cameraRotation_ = camera.GetRotation();
	ImGui::DragFloat3("CameraTranslate", &cameraPosition_.x, 0.01f);
	ImGui::DragFloat3("CameraRotate", &cameraRotation_.x, 0.01f);
	camera.SetPosition(cameraPosition_);
	camera.SetRotation(cameraRotation_);

	ImGui::End();
	// #endif

	#pragma endregion

	// 1つ目の球のみ入力で移動する
	spheres[0].UpdateToKeyMove(keys);

	// 衝突判定
	if (IsSpherePlaneCollision(spheres[0], planes[0])) {
		spheres[0].SetColor(0xFF0000FF);
	} else {
		spheres[0].SetColor(0xFFFFFFFF);
	}

	camera.Update(kWindowWidth, kWindowHeight, viewProjectionMatrix, viewportMatrix, keys);

	if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
		manager.SetScene(new TitleScene());
	}
}

void GameScene::Draw() {
	DrawGrid(viewProjectionMatrix, viewportMatrix);
	for (auto& sphere : spheres) {
		sphere.Draw(viewProjectionMatrix, viewportMatrix);
	}
	for (auto& plane : planes) {
		plane.Draw(viewProjectionMatrix, viewportMatrix);
	}
}

#pragma endregion
