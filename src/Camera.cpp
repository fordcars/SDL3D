#include <Camera.h>

void Camera::init() // Called when constructed, private
{
	// Default values
	mViewMatrix = glm::mat4(1.0f); // Identity matrix
	mProjectionMatrix = glm::mat4(1.0f);

	mPos = glm::vec3(0, 0, 0);
	mTarget = glm::vec4(0, 0, 0, 1); // 1 for position
	mUpVector = glm::vec3(0, 1, 0);

	mFieldOfView = 120.0f;
	mAspectRatio = 4/3;
	mNearClippingPlane = 0.1f;
	mFarClippingPlane = 100.0f;
}

Camera::Camera() // Constructor
{
	init();
}

Camera::Camera(float fieldOfView, float aspectRatio)
{
	init();

	mFieldOfView = fieldOfView;
	mAspectRatio = aspectRatio;
}

Camera::~Camera()
{
	// Do nothing
}
void Camera::setPos(glm::vec3 pos)
{
	mPos = pos;
}

void Camera::translate(glm::vec3 translation)
{
	mPos += translation;
}

glm::vec3 Camera::getPos()
{
	return mPos;
}

void Camera::setTarget(glm::vec4 target) // vec4 since this can be a position or a direction
{
	mTarget = target;
}

void Camera::setUpVector(glm::vec3 upVector)
{
	mUpVector = upVector;
}

void Camera::setFieldOfView(float fieldOfView) // In degrees
{
	mFieldOfView = fieldOfView;
}

void Camera::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
}

void Camera::updateMatrices() // Call after you are done setting up the camera
{
	glm::vec3 vec3Target;

	if(mTarget.w==1) // Is a position
		vec3Target = glm::vec3(mTarget); // Doesn't cast, it creates a new one
	else if(mTarget.w==0)
		vec3Target = glm::vec3(mTarget) + mPos; // Is a direction

	mViewMatrix = glm::lookAt(mPos, vec3Target, mUpVector);
	mProjectionMatrix = glm::perspective(mFieldOfView, mAspectRatio, mNearClippingPlane, mFarClippingPlane);
}

glm::mat4 Camera::getViewMatrix()
{
	return mViewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return mProjectionMatrix;
}