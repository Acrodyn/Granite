#pragma once

#include "GMath/FVector3.h"

namespace Granite
{
	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;

		static void SetMainCamera(Camera& camera, bool deleteOldCamera = false);
		static Camera* const GetMainCamera();

		GMath::FVector3 position;

	private:
		static Camera* mainCamera;
	};
}