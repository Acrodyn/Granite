#pragma once

#include "GGame/GObject.h"

class MainShip : public GObject
{
public:
	MainShip(std::string model, std::string texture = "");
	~MainShip() = default;

	virtual void Update(float deltaTime, const Uint8* state) override;
protected:
	void SetupView() override;

private:
	float _moveSpeed;

	float _yawCap;
	float _pitchCap;
};