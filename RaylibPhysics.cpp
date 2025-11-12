#include <memory>

#include "World.h"
#include "DemoWorld.h"

int main(void)
{
	auto App = std::make_unique<World>();
	auto DemoApp = std::make_unique<DemoWorld>();

	bool usingDemoApp = false;
	bool toggle = false;
	bool spawningCollidable = true;
	bool spawningStatic = false;

	if (usingDemoApp == false)
	{
		App->Init();

		while (!App->ShouldClose())
		{
			//Toggles Gravity
			if (IsKeyPressed(KEY_G))
			{
				if (toggle == true)
				{
					App->EnableAllGravity(true);
					toggle = false;
				}
				else if (toggle == false)
				{
					App->DisableAllGravity(true);
					toggle = true;
				}
			}
			//Turns off Gravity and stops all momentum
			if (IsKeyPressed(KEY_H))
			{
				App->DisableAllGravity(false);
				toggle = true;
			}

			//Makes a NONE/Pixel object
			if (IsKeyPressed(KEY_E))
			{
				App->MakeNewObject(1, spawningCollidable, spawningStatic);
			}
			//Makes a CIRCLE object
			else if (IsKeyPressed(KEY_Q))
			{
				App->MakeNewObject(2, spawningCollidable, spawningStatic);
			}
			//Makes an AABB/Rectangle Object
			else if (IsKeyPressed(KEY_W))
			{ 
				App->MakeNewObject(3, spawningCollidable, spawningStatic);
			}
			//Swaps between spawning collidable and not
			else if (IsKeyPressed(KEY_C))
			{
				spawningCollidable = !spawningCollidable;
			}
			//Swaps between spawning static and not
			else if (IsKeyPressed(KEY_SPACE))
			{
				spawningStatic = !spawningStatic;
			}
			//Attempt to pick up an object while left click is held
			if (IsMouseButtonDown(0))
			{
				App->PickupWithMouse();
			}
			if (IsMouseButtonReleased(0))
			{
				App->DropPickedUpObjects();
			}
			//Attempt to scale an object while right click is held
			if (IsMouseButtonDown(1))
			{
				App->ScaleWithMouse();
			}
			if (IsMouseButtonReleased(1))
			{
				App->StopScalingWithMouse();
			}

			App->Tick();
			while (App->ShouldTickFixed())
			{
				App->TickFixed();
			}
			App->Draw();
		}

		App->Exit();
	}
	else
	{
		DemoApp->Init();

		while (!DemoApp->ShouldClose())
		{
			DemoApp->Tick();
			while (DemoApp->ShouldTickFixed())
			{
				DemoApp->TickFixed();
			}
			DemoApp->Draw();
		}

		DemoApp->Exit();
	}
	

	return 0;
}