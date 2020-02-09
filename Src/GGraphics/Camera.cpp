#include "GGraphics/Camera.h"

namespace Granite
{
	Camera* Camera::mainCamera = nullptr;

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
}