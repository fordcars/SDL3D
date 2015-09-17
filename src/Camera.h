#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>

class Camera
{
private:
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix; // Gives perspective

	glm::vec3 mPos;
	glm::vec4 mTarget; // Target point or direction
	glm::vec3 mUpVector; // glm::vec3(0, -1, 0) will the camera be upside down, in contrast to glm::vec3(0, 1, 0);
						// This up vector represents the direction of the up side of the camera.

	float mFieldOfView; // In degrees
	float mAspectRatio;
	float mNearClippingPlane;
	float mFarClippingPlane;

	void init();

public:
	Camera();
	Camera(float fieldOfView, float aspectRatio);
	~Camera();

	void setPos(glm::vec3 pos);
	void translate(glm::vec3 translation);
	glm::vec3 getPos();

	void setTarget(glm::vec4 target);
	void setUpVector(glm::vec3 upVector);

	void setFieldOfView(float fieldOfView);
	void setAspectRatio(float aspectRatio);

	void updateMatrices();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
};

#endif /* CAMERA_H_ */