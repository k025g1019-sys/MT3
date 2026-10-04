#pragma once
#include "Camera.h"
#include "Matrix4x4.h"
#include "Objects.h"
#include "Spherical.h"
#include <memory>
#include <numbers>

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

class SceneManager;

// Scene
class Scene {
public:
	virtual ~Scene() = default;
	virtual void Update(SceneManager& manager) = 0;
	virtual void Draw() = 0;
};

// SceneManager
class SceneManager {
private:
	std::unique_ptr<Scene> current;
	std::unique_ptr<Scene> next;

public:
	~SceneManager();
	void SetScene(std::unique_ptr<Scene> scene);
	void Update();
	void Draw();
};

#pragma region TitleScene
// Title
class TitleScene : public Scene {
public:
	TitleScene();
	void Update(SceneManager& manager) override;
	void Draw() override;

private:
	char keys[256]{};
	char preKeys[256]{};

private:
	Vector3 c;
	Vector3 d;
	Vector3 e;
	Matrix4x4 rotateMatrix;
};
#pragma endregion

#pragma region GameScene
// Game
class GameScene : public Scene {
public:
	GameScene();
	void Update(SceneManager& manager) override;
	void Draw() override;

private:
	Camera camera;
	Objects *objects;

	Matrix4x4 viewProjectionMatrix = MakeIdentity4x4();
	Matrix4x4 viewportMatrix = MakeIdentity4x4();

	// 球面座標で表したカメラ位置(r=6, θ=0, φ=-π/2 → 直交座標(0, 0, -6))
	Spherical spherical{6.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f};

	char keys[256]{};
	char preKeys[256]{};
};
#pragma endregion