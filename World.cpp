#include <unordered_map> // Similar to a Dictionary
#include <iostream>
#include <glm/glm.hpp>
//#include <glm/gtx/norm.hpp>
#include "raylib.h"
#include "raymath.h"
#include "Shapes.h"
#include "EnumUtils.h"
#include "World.h"

using CollisionFunc = bool (*)(const glm::vec2&, const Shape&, const glm::vec2&, const Shape&);
using CollisionMap = std::unordered_map<ShapeType, CollisionFunc>;

// Function signature for resolution functions
using DepenetrationFunc = glm::vec2(*)(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen);

// Map for associating pairs of collision shapes with their resolution functions
using DepenetrationMap = std::unordered_map <ShapeType, DepenetrationFunc>;

CollisionMap ColMap;
DepenetrationMap DepenMap;

World::World() : AccumulatedFixedTime(0), TargetFixedStep(1.0f / 30.0f)
{
}

void World::Init()
{
    //const int screenWidth = 800;
    //const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    ColMap[ShapeType::CIRCLE | ShapeType::CIRCLE] = CheckCircleCircle;
    ColMap[ShapeType::AABB   | ShapeType::AABB]   = CheckAABBAABB;
    ColMap[ShapeType::CIRCLE | ShapeType::AABB]   = CheckCircleAABB;

    DepenMap[ShapeType::CIRCLE | ShapeType::CIRCLE] = DepenetrateCircleCircle;
    DepenMap[ShapeType::AABB | ShapeType::AABB] = DepenetrateAABBAABB;
    DepenMap[ShapeType::CIRCLE | ShapeType::AABB] = DepenetrateCircleAABB;

    ////Makes a starter circle object and adds it to the Objects array
    //std::shared_ptr<PhysObject> NewCirclePhysObject = std::make_shared<PhysObject>();
    //NewCirclePhysObject->ColliderShape.Type = ShapeType::CIRCLE;
    //NewCirclePhysObject->Position = glm::vec2(350, 225);
    //AddObjectToObjects(NewCirclePhysObject);

    ////// Makes a starter AABB/Rectangle object and adds it to the Objects array
    //std::shared_ptr<PhysObject> NewAABBPhysObject = std::make_shared<PhysObject>();
    //NewAABBPhysObject->ColliderShape.Type = ShapeType::AABB;
    //NewAABBPhysObject->ColliderShape.AABBData.HalfExtents = glm::vec2(10, 10);
    //NewAABBPhysObject->Position = glm::vec2(450, 215);
    //AddObjectToObjects(NewAABBPhysObject);

    //Mouse hitbox
    std::shared_ptr<PhysObject> MousePhysObject = std::make_shared<PhysObject>();
    MousePhysObject->ColliderShape.Type = ShapeType::CIRCLE;
    MousePhysObject->ColliderShape.CircleData.Radius = 5;
    MousePhysObject->SetHasGravity(false);
    MousePhysObject->Position = glm::vec2(400, 225);
    AddObjectToObjects(MousePhysObject);

    MouseHitbox = MousePhysObject;

    OnInit();
}

void World::Tick()
{
    AccumulatedFixedTime += GetFrameTime();

    OnTick();
}

void World::TickFixed()
{
    AccumulatedFixedTime -= TargetFixedStep;

    //Moves MouseHitbox to the mouse
    glm::vec2 MousePosition;
    MousePosition.x = GetMousePosition().x;
    MousePosition.y = GetMousePosition().y;
    MouseHitbox->Position = MousePosition;

    for (auto Objecti : Objects)
    {
        for (auto Objectj : Objects)
        {
            if (Objecti == Objectj) { continue; }
            if (Objecti->ColliderShape.Type == ShapeType::NONE || Objectj->ColliderShape.Type == ShapeType::NONE) { continue; }

            //If either object is pickedUp
            if (Objecti->pickedUp == true || Objectj->pickedUp == true)
            {
                //... but mouse is not picking up
                if (mousePickingUp == false)
                {
                    //... let them free
                    Objecti->pickedUp = false;
                    Objectj->pickedUp = false;
                    mouseHasAnObject = false;

                    if (mouseScaling == false)
                    {
                        if (gravityEnabled == true)
                        {
                            Objecti->SetHasGravity(true);
                            Objectj->SetHasGravity(true);
                        }
                    }
                }
                //... and the mouse is still picking up
                if (mousePickingUp == true)
                {
                    //... set the one that is being picked up to the mouse position if scaling isnt currently happening
                    if (Objecti->pickedUp == true)
                    {
                        Objecti->SetHasGravity(false);

                        if (mouseScaling == false)
                        {
                            float distance = glm::distance(MouseHitbox->Position, Objecti->Position);
                            glm::vec2 direction = MouseHitbox->Position - Objecti->Position;
                            glm::normalize(direction);
                            Objecti->Velocity = direction;
                        }
                    }
                    if (Objectj->pickedUp == true)
                    {
                        Objectj->SetHasGravity(false);

                        if (mouseScaling == false)
                        {
                            float distance = glm::distance(MouseHitbox->Position, Objectj->Position);
                            glm::vec2 direction = MouseHitbox->Position - Objectj->Position;
                            glm::normalize(direction);
                            Objectj->Velocity = direction;
                        }
                        
                    }
                }
            }
            //If either object is being scaled
            if (Objecti->beingScaled == true || Objectj->beingScaled == true)
            {
                //... but mouse is no longer scaling
                if (mouseScaling == false)
                {
                    //... let them free
                    Objecti->beingScaled = false;
                    Objectj->beingScaled = false;
                    if (mousePickingUp == false)
                    {
                        if (gravityEnabled == true)
                        {
                            Objecti->SetHasGravity(true);
                            Objectj->SetHasGravity(true);
                        }
                    }
                }
                //... and the mouse is still scaling
                if (mouseScaling == true)
                {
                    //... set the one that is being picked up to the mouse position if scaling isnt currently happening
                    if (Objecti->beingScaled == true)
                    {
                        Objecti->SetHasGravity(false);
                        Objecti->Velocity = glm::vec2(0, 0);

                        if (Objecti->ColliderShape.Type == ShapeType::CIRCLE)
                        {
                            float distance = glm::distance(MouseHitbox->Position, Objecti->Position);
                            Objecti->ColliderShape.CircleData.Radius = distance;
                        }
                        if (Objecti->ColliderShape.Type == ShapeType::AABB)
                        {
                            Objecti->ColliderShape.AABBData.HalfExtents = glm::vec2(
                                (MouseHitbox->Position.x - Objecti->Position.x),
                                (MouseHitbox->Position.y - Objecti->Position.y));
                        }
                    }
                    if (Objectj->beingScaled == true)
                    {
                        Objectj->SetHasGravity(false);
                        Objectj->Velocity = glm::vec2(0, 0);

                        if (Objectj->ColliderShape.Type == ShapeType::CIRCLE)
                        {
                            float distance = glm::distance(MouseHitbox->Position, Objectj->Position);
                            Objectj->ColliderShape.CircleData.Radius = distance;
                        }
                        if (Objectj->ColliderShape.Type == ShapeType::AABB)
                        {
                            Objectj->ColliderShape.AABBData.HalfExtents = glm::vec2(
                                (MouseHitbox->Position.x - Objectj->Position.x),
                                (MouseHitbox->Position.y - Objectj->Position.y));
                        }
                    }
                }
            }

            ShapeType ColKey = Objecti->ColliderShape.Type | Objectj->ColliderShape.Type;
            ShapeType PairType = Objecti->ColliderShape.Type | Objectj->ColliderShape.Type;

            auto KeyPair = ColMap.find(ColKey);

            bool HasFunc = KeyPair != ColMap.end();
            if (HasFunc)
            {
                bool IsColliding = false;

                if (Objecti->ColliderShape.Type > Objectj->ColliderShape.Type)
                {
                    IsColliding = ColMap[ColKey](Objectj->Position, Objectj->ColliderShape, Objecti->Position, Objecti->ColliderShape);
                }
                else
                {
                    IsColliding = ColMap[ColKey](Objecti->Position, Objecti->ColliderShape, Objectj->Position, Objectj->ColliderShape);
                }

                if (IsColliding == true)
                {
                    //If either object is the mouse
                    if (Objecti == MouseHitbox || Objectj == MouseHitbox)
                    {
                        //... and the mouse is picking up
                        if (mousePickingUp == true)
                        {
                            //... and the mouse does not currently have an object
                            if (mouseHasAnObject == false)
                            {
                                //... pick up the other object
                                if (Objecti == MouseHitbox)
                                {
                                    Objectj->pickedUp = true;
                                    mouseHasAnObject = true;
                                    Objectj->SetHasGravity(false);
                                }
                                else if (Objectj == MouseHitbox)
                                {
                                    Objecti->pickedUp = true;
                                    mouseHasAnObject = true;
                                    Objecti->SetHasGravity(false);
                                }
                            }
                        }
                        //... and the mouse is scaling
                        if (mouseScaling == true)
                        {
                            //... start scaling the other object
                            if (Objecti == MouseHitbox)
                            {
                                Objectj->beingScaled = true;
                                Objectj->SetHasGravity(false);
                                Objectj->Velocity = glm::vec2(0, 0);
                            }
                            else if (Objectj == MouseHitbox)
                            {
                                Objecti->beingScaled = true;
                                Objecti->SetHasGravity(false);
                                Objecti->Velocity = glm::vec2(0, 0);
                            }
                        }
                    }
                    else
                    {
                        //if both objects are being picked up ignore collision
                        if (Objecti->pickedUp && Objectj->pickedUp)
                        {

                        }
                        else
                        {
                            // Pen will be assigned a value by the depenetration func below, but it's
                            // good to have a default nonetheless
                            float Pen = 0.0f;

                            glm::vec2 Normal = DepenMap[PairType](Objecti->Position, Objecti->ColliderShape,
                                Objectj->Position, Objectj->ColliderShape, Pen);

                            Objecti->ResolvePhysObjects(*Objecti,   // first object
                                                        *Objectj,   // second object
                                                        1.0f,       // elasticity - hard-coded to 1 for now (could be configurable in World)
                                                        Normal,     // collision normal
                                                        Pen);       // penetration depth
                        }

                        //std::cout << " Collision: " << debugCollisionCounter << std::endl;
                        debugCollisionCounter++;
                    }
                }
            }
            else
            {
                std::cerr << "Unable to determine collision" << std::endl;
            }
        }

        Objecti->TickPhys(TargetFixedStep);

        //Loops objects around the screen if they go off
        if (Objecti->ColliderShape.Type == ShapeType::CIRCLE)
        {
            //Right side of screen to Left
            if (Objecti->Position.x > screenWidth + Objecti->ColliderShape.CircleData.Radius)
                Objecti->Position.x = 0 - Objecti->ColliderShape.CircleData.Radius;
            //Left side of screen to Right
            if (Objecti->Position.x < 0 - Objecti->ColliderShape.CircleData.Radius)
                Objecti->Position.x = screenWidth + Objecti->ColliderShape.CircleData.Radius;
            //Top of screen to Bottom
            if (Objecti->Position.y < 0 - Objecti->ColliderShape.CircleData.Radius)
                Objecti->Position.y = screenHeight + Objecti->ColliderShape.CircleData.Radius;
            //Bottom of screen to Top
            if (Objecti->Position.y > screenHeight + Objecti->ColliderShape.CircleData.Radius)
                Objecti->Position.y = 0 - Objecti->ColliderShape.CircleData.Radius;
        }
        else if (Objecti->ColliderShape.Type == ShapeType::AABB)
        {
            //Right side of screen to Left
            if (Objecti->Position.x > screenWidth + glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.x))
                Objecti->Position.x = 0 - glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.x);
            //Left side of screen to Right
            if (Objecti->Position.x < 0 - glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.x))
                Objecti->Position.x = screenWidth + glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.x);
            //Top of screen to Bottom
            if (Objecti->Position.y < 0 - glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.y))
                Objecti->Position.y = screenHeight + glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.y);
            //Bottom of screen to Top
            if (Objecti->Position.y > screenHeight + glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.y))
                Objecti->Position.y = 0 - glm::abs(Objecti->ColliderShape.AABBData.HalfExtents.y);
        }
        else
        {
            //Right side of screen to Left
            if (Objecti->Position.x > screenWidth + 5)
                Objecti->Position.x = -5;
            //Left side of screen to Right
            if (Objecti->Position.x < -5)
                Objecti->Position.x = screenWidth + 5;
            //Top of screen to Bottom
            if (Objecti->Position.y < -5)
                Objecti->Position.y = screenHeight + 5;
            //Bottom of screen to Top
            if (Objecti->Position.y > screenHeight + 5)
                Objecti->Position.y = -5;
        }
    }

    //Makes sure the mouse isn't affected by gravity and also does not have any velocity
    MouseHitbox->SetHasGravity(false);
    MouseHitbox->Velocity = glm::vec2(0,0);


    OnTickFixed();
}

void World::Draw() const
{
    BeginDrawing();

    ClearBackground(BLACK);

    for (auto Objecti : Objects)
    {
        Objecti->Draw();
    }

    //DrawText("Window Made!", 190, 200, 20, RED);

    OnDraw();

    EndDrawing();
}

void World::Exit()
{
    OnExit();

    CloseWindow();
}

bool World::ShouldClose() const
{
    return WindowShouldClose();
}

bool World::ShouldTickFixed() const
{
    return AccumulatedFixedTime >= TargetFixedStep;
}

//The Type is either 1: NONE, 2: CIRCLE, or 3: AABB
//There is an additonal "type" of 4: AABB but it has no gravity and is wider so it just acts like a platform
void World::MakeNewObject(int type)
{
    switch (type)
    {
    //NONE
    case 1:
    {
        std::shared_ptr<PhysObject> NewPhysObject = std::make_shared<PhysObject>();
        NewPhysObject->SetHasGravity(gravityEnabled);
        glm::vec2 MousePosition;
        MousePosition.x = GetMousePosition().x;
        MousePosition.y = GetMousePosition().y;
        NewPhysObject->Position = MousePosition;
        AddObjectToObjects(NewPhysObject);
        break;
    }

    //CIRCLE
    case 2:
    {

        std::shared_ptr<PhysObject> NewCIRCLEPhysObject = std::make_shared<PhysObject>();
        NewCIRCLEPhysObject->ColliderShape.Type = ShapeType::CIRCLE;
        NewCIRCLEPhysObject->SetHasGravity(gravityEnabled);
        glm::vec2 MousePosition;
        MousePosition.x = GetMousePosition().x;
        MousePosition.y = GetMousePosition().y;
        NewCIRCLEPhysObject->Position = MousePosition;
        AddObjectToObjects(NewCIRCLEPhysObject);
        break;
    }

    //AABB
    case 3:
    {

        std::shared_ptr<PhysObject> NewAABBPhysObject = std::make_shared<PhysObject>();
        NewAABBPhysObject->ColliderShape.Type = ShapeType::AABB;
        NewAABBPhysObject->ColliderShape.AABBData.HalfExtents = glm::vec2(10, 10);
        NewAABBPhysObject->SetHasGravity(gravityEnabled);
        glm::vec2 MousePosition;
        MousePosition.x = GetMousePosition().x;
        MousePosition.y = GetMousePosition().y;
        NewAABBPhysObject->Position = MousePosition;
        AddObjectToObjects(NewAABBPhysObject);
        break;
    }

    //PLATFORM
    case 4:
    {

        std::shared_ptr<PhysObject> NewAABBPhysObject = std::make_shared<PhysObject>();
        NewAABBPhysObject->ColliderShape.Type = ShapeType::AABB;
        NewAABBPhysObject->ColliderShape.AABBData.HalfExtents = glm::vec2(50, 10);
        NewAABBPhysObject->SetHasGravity(false);
        glm::vec2 MousePosition;
        MousePosition.x = GetMousePosition().x;
        MousePosition.y = GetMousePosition().y;
        NewAABBPhysObject->Position = MousePosition;
        AddObjectToObjects(NewAABBPhysObject);
        break;
    }
    }
}

void World::AddObjectToObjects(std::shared_ptr<PhysObject> ObjectToAdd)
{
    Objects.push_back(ObjectToAdd);
}

void World::DisableAllGravity(bool keepMomentum)
{
    gravityEnabled = false;
    for (auto Objecti : Objects)
    {
        Objecti->SetHasGravity(false);
        if (keepMomentum == false)
        {
            Objecti->Velocity = glm::vec2(0, 0);
        }
    }
}

void World::EnableAllGravity(bool keepMomentum)
{
    gravityEnabled = true;
    for (auto Objecti : Objects)
    {
        Objecti->SetHasGravity(true);
        if (keepMomentum == false)
        {
            Objecti->Velocity = glm::vec2(0, 0);
        }
    }
}

void World::PickupWithMouse()
{
    mousePickingUp = true;
}
void World::DropPickedUpObjects()
{
    mousePickingUp = false;
}

void World::ScaleWithMouse()
{
    mouseScaling = true;
}

void World::StopScalingWithMouse()
{
    mouseScaling = false;
}
