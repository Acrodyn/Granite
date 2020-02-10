#include "GGraphics/Camera.h"

namespace Granite
{
	Camera* Camera::mainCamera = nullptr;

	Camera::Camera()
	{
		forward = { 0, 0, 1 };
	}

	void Camera::SetMainCamera(Camera& camera, bool deleteOldCamera)
	{
		if (deleteOldCamera && mainCamera)
		{
			delete mainCamera;
		}

		mainCamera = &camera;
	}

	Camera* const Camera::GetMainCamera()
	{
		return mainCamera;
	}

	GMath::FVector3 Camera::GetTarget() const
	{
		return position + forward;
	}
}