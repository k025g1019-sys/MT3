#include "Objects.h"
#include "AABB.h"
#include "OBB.h"
#include "Collision.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "Segment.h"
#include "Sphere.h"
#include "Triangle.h"
#ifdef _DEBUG
#include <imgui.h>
#endif

Objects::Objects() {
	spheres = {
	    Sphere({0.0f, 0.0f, 0.0f}, 0.5f),
	    //Sphere({0.8f, 0.0f, 1.0f}, 0.4f),
	};

	planes = {
	    //Plane({0.0f, 1.0f, 0.0f}, 1.5f),
	};

	segments = {
	    //Segment({-0.7f, 0.3f, 0.0f}, {2.0f, -0.5f, 0.0f}),
	};

	triangles = {
	    //Triangle(Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)),
	};

	aabbs = {
	    //AABB({-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}),
	    //AABB({0.2f, 0.2f, 0.2f}, {1.0f, 1.0f, 1.0f}),
	};

	obbs = {
	    OBB({-1.0f, 0.0f, 0.0f},
        {{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}},
        {0.5f, 0.5f, 0.5f}
        ),
	};
}

#pragma region Collisions

#pragma region SphereSphere
// 球と球の衝突判定
void Objects::UpdateCollisionSphereSphere() {
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
	}
}
#pragma endregion

#pragma region SpherePlane
// 球と平面の衝突判定
void Objects::UpdateCollisionSpherePlane() {
	for (auto& sphere : spheres) {
		bool isHit = false;

		// 平面との判定
		for (auto& plane : planes) {
			if (IsSpherePlaneCollision(sphere, plane)) {
				isHit = true;
				break;
			}
		}

		sphere.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}
}
#pragma endregion

#pragma region SegmentPlane
// 線と平面の衝突判定
void Objects::UpdateCollisionSegmentPlane() {
	for (auto& segment : segments) {
		bool isHit = false;

		// 平面との判定
		for (auto& plane : planes) {
			if (IsSegmentPlaneCollision(segment, plane)) {
				isHit = true;
				break;
			}
		}
	}
}
#pragma endregion

#pragma region SegmentTriangle
// 線と三角形の衝突判定
void Objects::UpdateCollisionSegmentTriangle() {
	for (auto& segment : segments) {
		bool isHit = false;

		// 三角形との判定
		for (auto& triangle : triangles) {
			if (IsTriangleSegmentCollision(triangle, segment)) {
				isHit = true;
				break;
			}
		}

		segment.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}
}
#pragma endregion

#pragma region AABBs
// AABB同士の衝突判定
void Objects::UpdateCollisionAABBs() {
	for (auto& aabb : aabbs) {
		bool isHit = false;

		// AABB同士
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
	}
}
#pragma endregion

#pragma region AABBSphere
// AABBと球の衝突判定
void Objects::UpdateCollisionAABBSphere() {
	for (auto& aabb : aabbs) {
		bool isHit = false;

		AABB worldAABB = aabb.GetWorldAABB();

		// 球との判定
		for (auto& sphere : spheres) {
			if (IsAABBSphereCollision(worldAABB, sphere)) {
				isHit = true;
				break;
			}
		}

		aabb.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}
}
#pragma endregion

#pragma region AABBSegment
// AABBと線の衝突判定
void Objects::UpdateCollisionAABBSegment() {
	for (auto& aabb : aabbs) {
		bool isHit = false;

		AABB worldAABB = aabb.GetWorldAABB();

		// 線との判定
		for (auto& segment : segments) {
			if (IsAABBSegmentCollision(worldAABB, segment)) {
				isHit = true;
				break;
			}
		}

		aabb.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}
}
#pragma endregion

#pragma region OBBSphere
// OBBと球の衝突判定
void Objects::UpdateCollisionOBBSphere() {
	for (auto& obb : obbs) {
		bool isHit = false;

		// 球との判定
		for (auto& sphere : spheres) {
			if (IsOBBSphereCollision(obb, sphere)) {
				isHit = true;
				break;
			}
		}

		obb.SetColor(isHit ? 0xFF0000FF : 0xFFFFFFFF);
	}
}
#pragma endregion

#pragma endregion

void Objects::UpdateAllCollisions() {
	for (auto& aabb : aabbs) {
		aabb.Update();
	}
	for (auto& obb : obbs) {
		obb.Update();
	}
	// All Collisions
	UpdateCollisionSphereSphere();
	UpdateCollisionSpherePlane();
	UpdateCollisionSegmentPlane();
	UpdateCollisionSegmentTriangle();
	UpdateCollisionAABBs();
	UpdateCollisionAABBSphere();
	UpdateCollisionAABBSegment();
	UpdateCollisionOBBSphere();
}

#pragma region Draw
void Objects::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
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
	for (auto& obb : obbs) {
		obb.Draw(viewProjectionMatrix, viewportMatrix);
	}
}
#pragma endregion

#ifdef _DEBUG

#pragma region ImGui
void Objects::DrawImgui() {
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

#pragma region OBB
	// ---- OBB ----
	if (ImGui::TreeNode("OBBs")) {
		for (size_t i = 0; i < obbs.size(); ++i) {
			ImGui::PushID((int)i);

			ImGui::Text("OBB[%zu]", i);
			obbs[i].DrawImGui();

			ImGui::PopID();
		}
		ImGui::TreePop();
	}
#pragma endregion

	ImGui::End();
}
#pragma endregion

#endif