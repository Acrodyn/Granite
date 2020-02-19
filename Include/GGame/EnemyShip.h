#pragma once

#include "GGame/GObject.h"

class EnemyShip : public GObject
{
public:
	EnemyShip(std::string model, std::string texture = "");
	~EnemyShip() = default;

	virtual void Update(float deltaTime, const Uint8* state) override;
private:
};