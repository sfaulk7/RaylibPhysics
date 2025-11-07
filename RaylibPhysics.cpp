#include <memory>

#include "World.h"
#include "DemoWorld.h"

int main(void)
{
	auto App = std::make_unique<World>();
	auto DemoApp = std::make_unique<DemoWorld>();

	bool usingDemoApp = false;
	bool toggle = false;

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
			if (IsKeyPressed(KEY_R))
			{
				App->MakeNewObject(1);
			}
			//Makes a CIRCLE object
			else if (IsKeyPressed(KEY_Q))
			{
				App->MakeNewObject(2);
			}
			//Makes an AABB/Rectangle Object
			else if (IsKeyPressed(KEY_E))
			{ 
				App->MakeNewObject(3);
			}
			//Makes an AABB/Platform object unaffected by gravity
			else if (IsKeyPressed(KEY_SPACE))
			{
				App->MakeNewObject(4);
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