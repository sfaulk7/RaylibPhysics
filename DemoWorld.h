#pragma once

#include "World.h"
class DemoWorld : public World
{
public:
	void OnTick() override;
	void OnDraw() const override;
};

