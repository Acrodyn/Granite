#pragma once

#include "GMath/FVector3.h"

namespace Granite
{
	class Camera
	{
	public:
		Camera();
		~Camera() = default;

		static void SetMainCamera(Camera& camera, bool deleteOldCamera = false);
		static Camera* const GetMainCamera();

		GMath::FVector3 GetTarget() const;

		GMath::FVector3 position;
		GMath::FVector3 forward;

	private:
		static Camera* mainCamera;
	};
}