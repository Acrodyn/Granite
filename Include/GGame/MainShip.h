#pragma once

#include "GGame/GObject.h"

class MainShip : public GObject
{
public:
	MainShip(std::string model, std::string texture = "");
	~MainShip() = default;

	virtual void Update(float deltaTime) override;
protected:
	void SetupView() override;

private:
	bool _UpdateBasicMovement(Granite::GMath::FMatrix4x4 &transform, float deltaTime);
	void _StabilizeShip(Granite::GMath::FMatrix4x4& transform, float deltaTime);

	float _moveSpeed;
	float _positionSpeed;
	float _adjustmentSpeed;
	float _yawCap;
	float _pitchCap;

	Granite::GMath::FMatrix4x4 defaultState;
};