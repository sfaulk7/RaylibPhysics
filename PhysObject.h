#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include "Shapes.h"
#include "raylib.h"

class PhysObject
{
public:
	PhysObject();

	glm::vec2 Position;
	glm::vec2 Velocity;
	glm::vec2 Acceleration;
	float Mass;
	glm::vec2 Drag;
	glm::vec2 Gravity;
	bool hasGravity;
	bool isStatic;
	bool isCollidable;

	Shape ColliderShape;

	bool pickedUp = false;
	bool beingScaled = false;

public:
	void TickPhys(float Delta);

	void AddAcceleration(glm::vec2 AccelerationToAdd);
	void AddVelocity(glm::vec2 VelocityToAdd);
	void AddForce(glm::vec2 ForceToAdd);
	void AddImpulse(glm::vec2 ImpulseToAdd);

	// Calculates an impulse to object A and B, assuming they are in collision
	float ResolveCollision(const glm::vec2& PosA, const glm::vec2& VelA, float MassA,
		const glm::vec2& PosB, const glm::vec2& VelB, float MassB,
		float Elasticity, const glm::vec2& Normal);
	// Resolves a collision between two PhysObject instances
	void ResolvePhysObjects(PhysObject& Lhs, PhysObject& Rhs, float Elasticity, const glm::vec2& Normal, float Pen);

	void SetHasGravity(bool newValue);
	void SetIsCollidable(bool newValue);
	void SetIsStatic(bool newValue);

	void Draw() const;
};

