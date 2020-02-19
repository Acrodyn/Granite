#pragma once

#include "GGame/GObject.h"

class MainShip : public GObject
{
public:
	MainShip(std::string model, std::string texture = "");
	~MainShip() = default;

	virtual void Update(float deltaTime, const Uint8* state) override;
private:
};