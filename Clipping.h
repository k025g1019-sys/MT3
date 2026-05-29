#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"

Vector4 TransformToClip(const Vector3& v, const Matrix4x4& m);

bool ClipLine(Vector4& a, Vector4& b);

bool DrawClippedLine(const Vector3& start, const Vector3& end, const Matrix4x4& viewProjection, const Matrix4x4& viewport, unsigned int color);