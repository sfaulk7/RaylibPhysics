#pragma once
#include "PhysObject.h"
#include <vector>
#include <memory>


class World
{
protected:
	std::vector<std::shared_ptr<class PhysObject>> Objects;
	std::shared_ptr<class PhysObject> MouseHitbox;
	float AccumulatedFixedTime;

	int screenWidth = 1600;
	int screenHeight = 900;

public:
	World();

	void Init();
	void Tick();
	void TickFixed();
	void Draw() const;
	void Exit();

	bool ShouldClose() const;
	bool ShouldTickFixed() const;

	float TargetFixedStep;
	bool mousePickingUp = false;
	bool mouseScaling = false;
	int debugCollisionCounter = 0;

	//The Type is either 1: NONE, 2: CIRCLE, or 3: AABB
	void MakeNewObject(int type);
	void AddObjectToObjects(std::shared_ptr<PhysObject> ObjectToAdd);
	void DisableAllGravity(bool keepMomentum);
	void EnableAllGravity(bool keepMomentum);

	void PickupWithMouse();
	void DropPickedUpObjects();
	void ScaleWithMouse();
	void StopScalingWithMouse();

protected:
	virtual void OnInit() {}
	virtual void OnTick() {}
	virtual void OnTickFixed() {}
	virtual void OnDraw() const {}
	virtual void OnExit() {}
};

