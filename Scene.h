#pragma once
#include "Camera.h"
#include "Matrix4x4.h"
#include "Structure.h"
#include <vector>

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
	Scene* current = nullptr;
	Scene* next = nullptr;

public:
	~SceneManager();
	void SetScene(Scene* scene);
	void Update();
	void Draw();
};

// Title
class TitleScene : public Scene {
public:
	TitleScene();
	void Update(SceneManager& manager) override;
	void Draw() override;

private:
	char keys[256]{};
	char preKeys[256]{};
};

// Game
class GameScene : public Scene {
public:
	GameScene();
	void Update(SceneManager& manager) override;
	void Draw() override;

private:
	Camera camera;
	std::vector<Sphere> spheres;
	std::vector<Plane> planes;

	Matrix4x4 viewProjectionMatrix = MakeIdentity4x4();
	Matrix4x4 viewportMatrix = MakeIdentity4x4();

	char keys[256]{};
	char preKeys[256]{};
};