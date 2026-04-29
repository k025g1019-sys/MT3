#include "Scene.h"
#include "Structure.h"
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
	    //Sphere({.center = {0.0f, 0.5f, -2.0f}, .radius = 0.5f, .moveSpeed = 0.03f}
        //),
    };

	planes = {
	    Plane({.normal = {0.0f, 1.0f, 0.0f}, .distance = 1.5f}
		),
	};

	segments = {
	    Segment({.origin = {-0.45f, 0.41f, 0.0f}, .diff = {1.0f, 0.58f, 0.0f}}
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

	ImGui::PushID("spheres");
	for (size_t i = 0; i < spheres.size(); ++i) {
		ImGui::PushID((int)i);
		ImGui::Text("Sphere[%zu]", i);

		Vector3 sphereCenter = spheres[i].GetCenter();
		float sphereRadius = spheres[i].GetRadius();
		ImGui::DragFloat3("Center", &sphereCenter.x, 0.01f);
		ImGui::DragFloat("Radius", &sphereRadius, 0.01f);
		spheres[i].SetCenter(sphereCenter);
		spheres[i].SetRadius(sphereRadius);

		ImGui::PopID();
	}
	ImGui::PopID();

	ImGui::PushID("planes");
	for (size_t i = 0; i < planes.size(); ++i) {
		ImGui::PushID((int)i);
		ImGui::Text("plane[%zu]", i);

		Vector3 planeNormal = planes[i].GetNormal();
		float planeDistance = planes[i].GetDistance();
		ImGui::DragFloat3("Normal", &planeNormal.x, 0.01f);
		ImGui::DragFloat("Distance", &planeDistance, 0.01f);
		planes[i].SetNormal(planeNormal);
		planes[i].SetDistance(planeDistance);

		ImGui::PopID();
	}
	ImGui::PopID();

	ImGui::PushID("segments");
	for (size_t i = 0; i < segments.size(); ++i) {
		ImGui::PushID((int)i);
		ImGui::Text("segment[%zu]", i);

		Vector3 segmentsOrigin = segments[i].GetOrigin();
		Vector3 segmentsDiff = segments[i].GetDiff();
		ImGui::DragFloat3("Origin", &segmentsOrigin.x, 0.01f);
		ImGui::DragFloat3("Diff", &segmentsDiff.x, 0.01f);
		segments[i].SetOrigin(segmentsOrigin);
		segments[i].SetDiff(segmentsDiff);

		ImGui::PopID();
	}
	ImGui::PopID();

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
	if (!spheres.empty()) {
		spheres[0].UpdateToKeyMove(keys);
	}

	#pragma region 衝突判定

	// 球と球、球と平面の衝突判定
	for (auto& sphere : spheres) {
		bool isHit = false;

		// 球同士
		for (auto& other : spheres) {
			if (&sphere == &other)
				continue;

			Vector3 diff = Subtract(sphere.GetCenter(), other.GetCenter());
			float distance = Length(diff);
			float radiusSum = sphere.GetRadius() + other.GetRadius();

			if (distance <= radiusSum) {
				isHit = true;
				break;
			}
		}

		// 平面
		for (auto& plane : planes) {
			if (IsSpherePlaneCollision(sphere, plane)) {
				isHit = true;
				break;
			}
		}

		sphere.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}

	// 線と平面の衝突判定
	for (auto& segment : segments) {
		bool isHit = false;
		for (auto& plane : planes) {
			if (IsSegmentPlaneCollision(segment, plane)) {
				isHit = true;
				break;
			}
		}
		segment.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}

	#pragma endregion

	camera.Update(kWindowWidth, kWindowHeight, viewProjectionMatrix, viewportMatrix, keys);

	if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
		manager.SetScene(new TitleScene());
	}
}

// 描画
void GameScene::Draw() {
	DrawGrid(viewProjectionMatrix, viewportMatrix);
	for (auto& sphere : spheres) {
		sphere.Draw(viewProjectionMatrix, viewportMatrix);
	}
	for (auto& plane : planes) {
		plane.Draw(viewProjectionMatrix, viewportMatrix);
	}
	for (auto& segment : segments) {
		segment.Draw(viewProjectionMatrix, viewportMatrix);
	}
}

#pragma endregion
