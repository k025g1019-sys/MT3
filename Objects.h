#pragma once
#include <vector>

struct Matrix4x4;
class Sphere;
class Plane;
class Segment;
class Triangle;
class AABB;

class Objects {
public:
	Objects();
	void UpdateAllCollisions();
	void UpdateCollisionSphereSphere();
	void UpdateCollisionSpherePlane();
	void UpdateCollisionSegmentPlane();
	void UpdateCollisionSegmentTriangle();
	void UpdateCollisionAABBs();
	void UpdateCollisionAABBSphere();
	void UpdateCollisionAABBSegment();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImgui();
#endif

private:
	std::vector<Sphere> spheres;
	std::vector<Plane> planes;
	std::vector<Segment> segments;
	std::vector<Triangle> triangles;
	std::vector<AABB> aabbs;
};