#include "Scene.h"
#include "AABB.h"
#include "Collision.h"
#include "DebugGrid.h"
#include "Plane.h"
#include "Segment.h"
#include "Sphere.h"
#include "Triangle.h"
#include <Novice.h>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region SceneManager

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

	spheres = {
	    // Sphere({0.12f, 0.0f, 0.0f}, 0.6f),
	    // Sphere({0.8f, 0.0f, 1.0f}, 0.4f),
	};

	planes = {
	    // Plane({0.0f, 1.0f, 0.0f}, 1.5f),
	};

	segments = {
	    Segment({-0.0f, 0.5f, -1.0f}, {0.0f, 0.5f, 0.2f}),
	};

	triangles = {
	    Triangle(Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)),
	};

	aabbs = {
	    // AABB({-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}),
	    // AABB({0.2f, 0.2f, 0.2f}, {1.0f, 1.0f, 1.0f}),
	};

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

	// 線と平面、線と三角形の衝突判定
	for (auto& segment : segments) {
		bool isHit = false;

		// 平面との判定
		for (auto& plane : planes) {
			if (IsSegmentPlaneCollision(segment, plane)) {
				isHit = true;
				break;
			}
		}

		// 三角形との判定
		for (auto& triangle : triangles) {
			if (IsTriangleSegmentCollision(triangle, segment)) {
				isHit = true;
				break;
			}
		}

		segment.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}

	// AABB衝突判定
	for (auto& aabb : aabbs) {
		bool isHit = false;

		for (auto& other : aabbs) {
			if (&aabb == &other)
				continue;

			// 衝突判定
			AABB wa = aabb.GetWorldAABB();
			AABB wb = other.GetWorldAABB();

			if (IsAABBCollision(wa, wb)) {
				isHit = true;
				break;
			}
		}
		aabb.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}

#pragma endregion

	camera.Update(kWindowWidth, kWindowHeight, viewProjectionMatrix, viewportMatrix, keys);
}
#pragma endregion

#pragma region Draw
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
	for (auto& triangle : triangles) {
		triangle.Draw(viewProjectionMatrix, viewportMatrix);
	}
	for (auto& aabb : aabbs) {
		aabb.Draw(viewProjectionMatrix, viewportMatrix);
	}
#pragma region ImGui

#ifdef _DEBUG
	ImGui::Begin("window");

#pragma region Sphere
	// ---- Sphere ----
	if (ImGui::TreeNode("Spheres")) {
		for (size_t i = 0; i < spheres.size(); ++i) {
			ImGui::PushID((int)i);

			ImGui::Text("Sphere[%zu]", i);
			spheres[i].DrawImGui();

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
#pragma endregion

#pragma region Plane
	// ---- Plane ----
	if (ImGui::TreeNode("Planes")) {
		for (size_t i = 0; i < planes.size(); ++i) {
			ImGui::PushID((int)i);

			ImGui::Text("Plane[%zu]", i);
			planes[i].DrawImGui();

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
#pragma endregion

#pragma region Segment
	// ---- Segment ----
	if (ImGui::TreeNode("Segments")) {
		for (size_t i = 0; i < segments.size(); ++i) {
			ImGui::PushID((int)i);

			ImGui::Text("Segment[%zu]", i);
			segments[i].DrawImGui();

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
#pragma endregion

#pragma region Segment
	// ---- Triangle ----
	if (ImGui::TreeNode("Triangles")) {
		for (size_t i = 0; i < triangles.size(); ++i) {
			ImGui::PushID((int)i);

			ImGui::Text("Triangle[%zu]", i);
			triangles[i].DrawImGui();

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
#pragma endregion

#pragma region AABB
	// ---- AABB ----
	if (ImGui::TreeNode("AABBs")) {
		for (size_t i = 0; i < aabbs.size(); ++i) {
			ImGui::PushID((int)i);

			ImGui::Text("AABB[%zu]", i);
			aabbs[i].DrawImGui();

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
#pragma endregion

	ImGui::End();

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
