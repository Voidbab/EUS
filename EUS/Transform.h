#pragma once
#include "GLM.h"

typedef glm::vec3 Vector3;
typedef glm::mat4 Matrix4;

class Transform {
	Vector3 position;

	Matrix4 transform;
	Matrix4 scale;
	Matrix4 rotation;
public:
	Transform();

	void setPosition(const Vector3& position);
	void setX(const float x);
	void setY(const float y);
	void setZ(const float z);

	const float getX() const;
	const float getY() const;
	const float getZ() const;

	Vector3 getPosition() const;

	void setRotation(const float r, const Vector3& axis);
	Matrix4 getRotation() const;

	void lookAt(const Vector3& vector);
	void translate(const Vector3& vector);

	Matrix4 getTransform() const;

	void setScale(const float value);

	~Transform();
};

