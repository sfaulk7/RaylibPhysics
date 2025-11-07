#pragma once

#include "Shapes.h"
#include "raylib.h"
#include <glm/vec2.hpp>

class PhysObject
{
public:
	PhysObject();

	glm::vec2 Position;
	glm::vec2 Velocity;
	glm::vec2 Acceleration;
	glm::vec2 Gravity;
	float Mass;
	float Drag;
	bool hasGravity;

	Shape ColliderShape;

	bool pickedUp = false;
	bool beingScaled = false;

public:
	void TickPhys(float Delta);

	void AddAcceleration(glm::vec2 AccelerationToAdd);
	void AddVelocity(glm::vec2 VelocityToAdd);
	void AddForce(glm::vec2 ForceToAdd);
	void AddImpulse(glm::vec2 ImpulseToAdd);

	void SetHasGravity(bool newValue);

	void Draw() const;
};

