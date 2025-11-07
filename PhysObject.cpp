#include "PhysObject.h"
#include <iostream>
#include <cstdlib>

PhysObject::PhysObject() :
	Position({ 0, 0 }),
	Velocity({ 0, 0 }),
	Acceleration({ 0, 0 }),
	Gravity({ 0, 0.1 }),
	Mass(10),
	Drag(Mass * 0.1f),
	hasGravity(true),
	ColliderShape({ShapeType::NONE})
{
}

void PhysObject::TickPhys(float Delta)
{
	// REMEMBER (0, 0) IN RAYLIB IS THE TOP LEFT
	// SO MOVING UPWARD IS ACTUALLY NEGATIVE AND MOVING DOWNWARD IS POSITIVE
	// SO PRETTY MUCH JUST INVERSE THE Y-AXIS VALUES

	//Moves position based on velocity
	Position += Velocity;

	//Increases velocity due to acceleration then decreases acceleration
	Velocity = Velocity + Acceleration;
	if (Acceleration.x > 0 || Acceleration.y > 0)
	{
		Acceleration.x = Acceleration.x - Drag;
		Acceleration.y = Acceleration.y - Drag;
		if (Acceleration.x < 0)
			Acceleration.x = 0;
		if (Acceleration.y < 0)
			Acceleration.y = 0;
	}
	else if (Acceleration.x < 0 || Acceleration.y < 0)
	{
		Acceleration.x = Acceleration.x + Drag;
		Acceleration.y = Acceleration.y + Drag;
		if (Acceleration.x > 0)
			Acceleration.x = 0;
		if (Acceleration.y > 0)
			Acceleration.y = 0;
	}
	

	//Adds gravity to velocity if it is enabled
	if (hasGravity == true)
	{
		if (!(Velocity.y >= Mass * 5))
			Velocity = Velocity + (Gravity);
	}

	//system("cls");
	//std::cout << "Position X: " << Position.x <<
	//	"\nPosition Y:" << Position.y << std::endl;
	//if (Velocity.x > 0 || Velocity.y > 0 || Velocity.x < 0 || Velocity.y < 0)
	//std::cout << "\rVelocity X: " << Velocity.x << " Velocity Y:" << Velocity.y << std::endl;
	//if (Acceleration.x > 0 || Acceleration.y > 0 || Acceleration.x < 0 || Acceleration.y < 0)
	//	std::cout << "\rAcceleration X: " << Acceleration.x << " Acceleration Y:" << Acceleration.y << std::endl;
}

// For appliying continuous forces
void PhysObject::AddAcceleration(glm::vec2 AccelerationToAdd)
{
	Acceleration = Acceleration + AccelerationToAdd;
}
// For applying instantaneous forces
void PhysObject::AddVelocity(glm::vec2 VelocityToAdd)
{
	Velocity += VelocityToAdd;
}
// For applying continuous forces that consider the object's mass
void PhysObject::AddForce(glm::vec2 ForceToAdd)
{
	Acceleration = ForceToAdd / Mass;
}
// For applying instantatneous forces that consider the object's mass
void PhysObject::AddImpulse(glm::vec2 ImpulseToAdd)
{
	Velocity += ImpulseToAdd / Mass;
}

void PhysObject::SetHasGravity(bool newValue)
{
	hasGravity = newValue;
}

void PhysObject::Draw() const
{
	switch (ColliderShape.Type)
	{
		case ShapeType::NONE:
			// here's a freebie: we'll just draw shapeless colliders as a point
			DrawPixel(Position.x, Position.y, RAYWHITE);
			break;
		case ShapeType::CIRCLE:
			// TODO: draw circle
			DrawCircleLines(Position.x, Position.y,
				ColliderShape.CircleData.Radius,
				BLUE);
			break;
		case ShapeType::AABB:
			// TODO: draw AABB
			// AABBs or Rectangles ALWAYS draw from the top left in raylib not the center
			// So we put its position to be offset by the HalfExtents of the width and height to make the center of the rectangle to be the actual point
			DrawRectangleLines(Position.x - (ColliderShape.AABBData.HalfExtents.x),
				Position.y - (ColliderShape.AABBData.HalfExtents.y),
				ColliderShape.AABBData.HalfExtents.x * 2.0f, ColliderShape.AABBData.HalfExtents.y * 2.0f,
				RED);
			break;
		default:
			break;
	}
}
