#pragma once

#include "GGame/GObject.h"

class SimpleGObject : public GObject
{
public:
	SimpleGObject(std::string model, std::string texture = "");
	~SimpleGObject() = default;

	virtual void Update(float deltaTime) override;
protected:
	void SetupView() override;
};