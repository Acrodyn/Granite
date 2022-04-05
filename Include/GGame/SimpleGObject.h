#pragma once

#include "GGame/GObject.h"

class SimpleGObject : public GObject
{
public:
	SimpleGObject(std::string model, std::string texture = "");
	~SimpleGObject() = default;

	virtual void Update(float deltaTime) override;
	void SetRotationSpeed(float rotationSpeed);
protected:
	void SetupView() override;
private:
	void Rotate(Granite::GMath::FMatrix4x4& transform, float deltaTime);

private:
	float _rotationSpeed = 0.f;
};