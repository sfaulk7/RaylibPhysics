#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include "Shapes.h"

bool CheckCircleCircle(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const Circle& CircleB)
{
    //Distance between circles
    glm::vec2 Offset = PosA - PosB;
    float Distance = glm::length(Offset);

    //Sum of radii
    float Sum = CircleA.Radius + CircleB.Radius;

    //They are in collision if the Distance < Sum
    return Distance < Sum;
}

// Shape wrapper for CIRCLE-CIRCLE collisions
bool CheckCircleCircle(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB)
{
    assert(ShapeA.Type == ShapeType::CIRCLE && "Called CheckCircleCircle but ShapeA is not a CIRCLE");
    assert(ShapeB.Type == ShapeType::CIRCLE && "Called CheckCircleCircle but ShapeB is not a CIRCLE");

    return CheckCircleCircle(PosA, ShapeA.CircleData, PosB, ShapeB.CircleData);
}

bool CheckAABBAABB(const glm::vec2& PosA, const AABB& AABBA, const glm::vec2& PosB, const AABB& AABBB)
{
    return PosA.x - glm::abs(AABBA.HalfExtents.x) < PosB.x + glm::abs(AABBB.HalfExtents.x) && // LEFT within RIGHT
           PosA.x + glm::abs(AABBA.HalfExtents.x) > PosB.x - glm::abs(AABBB.HalfExtents.x) && // RIGHT within LEFT
           PosA.y - glm::abs(AABBA.HalfExtents.y) < PosB.y + glm::abs(AABBB.HalfExtents.y) && // TOP within BOTTOM
           PosA.y + glm::abs(AABBA.HalfExtents.y) > PosB.y - glm::abs(AABBB.HalfExtents.y);   // BOTTOM within TOP
}
// Shape wrapper for AABB-AABB collisions
bool CheckAABBAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB)
{
    assert(ShapeA.Type == ShapeType::AABB && "Called CheckAABBAABB but ShapeA is not an AABB");
    assert(ShapeB.Type == ShapeType::AABB && "Called CheckAABBAABB but ShapeB is not an AABB");

    return CheckAABBAABB(PosA, ShapeA.AABBData, PosB, ShapeB.AABBData);
}

bool CheckCircleAABB(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const AABB& AABBB)
{
    //Distance between clamped point and circle
    float DistX = PosA.x - glm::clamp(PosA.x, PosB.x - glm::abs(AABBB.HalfExtents.x), PosB.x + glm::abs(AABBB.HalfExtents.x));
    float DistY = PosA.y - glm::clamp(PosA.y, PosB.y - glm::abs(AABBB.HalfExtents.y), PosB.y + glm::abs(AABBB.HalfExtents.y));

    //They are in collision if the Distance < Sum
    return (DistX * DistX + DistY * DistY) < CircleA.Radius * CircleA.Radius;
}
// Shape wrapper for CIRCLE-AABB collisions
bool CheckCircleAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB)
{
    assert(ShapeA.Type == ShapeType::CIRCLE && "Called CheckCircleAABB but ShapeA is not a CIRCLE");
    assert(ShapeB.Type == ShapeType::AABB && "Called CheckCircleAABB but ShapeB is not an AABB");

    return CheckCircleAABB(PosA, ShapeA.CircleData, PosB, ShapeB.AABBData);
}

glm::vec2 DepenetrateCircleCircle(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const Circle& CircleB, float& Pen)
{
    // get the distance between the two circles
    float Dist = glm::length(PosB - PosA);
    // add up the sum of the two radii
    float Radii = CircleA.Radius + CircleB.Radius;

    // find the difference and write it to the referenced variable
    Pen = Radii - Dist;

    // return the direction to correct along
    return glm::normalize(PosB - PosA);
}

glm::vec2 DepenetrateCircleCircle(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen)
{
    return DepenetrateCircleCircle(PosA, ShapeA.CircleData, PosB, ShapeB.CircleData, Pen);
}

glm::vec2 DepenetrateAABBAABB(const glm::vec2& PosA, const AABB& AABBA, const glm::vec2& PosB, const AABB& AABBB, float& Pen)
{
    float penX = 10;
    float penY = 10;
    float penYA = 10;
    float penYB = 10;
    float penXA = 10;
    float penXB = 10;
    bool AAboveB;
    bool ALeftOfB;

    if (PosA.y < PosB.y) //Due to raylib drawing from top left, having a higher y makes something be BELOW the other
    {
        AAboveB = true;
    }
    else
    {

        AAboveB = false;
    }
    if (PosA.x < PosB.x) //Though x is not affected
    {
        ALeftOfB = true;
    }
    else
    {
        ALeftOfB = false;
    }

    float ATopRightY = PosA.y + glm::abs(AABBA.HalfExtents.x) - glm::abs(AABBA.HalfExtents.y);
    float ABottomRightY = PosA.y + glm::abs(AABBA.HalfExtents.x) + glm::abs(AABBA.HalfExtents.y);
    float BTopRightY = PosB.y + glm::abs(AABBB.HalfExtents.x) - glm::abs(AABBB.HalfExtents.y);
    float BBottomRightY = PosB.y + glm::abs(AABBB.HalfExtents.x) + glm::abs(AABBB.HalfExtents.y);

    float ATopRightX = PosA.x + glm::abs(AABBA.HalfExtents.x) - glm::abs(AABBA.HalfExtents.y);
    float ABottomRightX = PosA.x + glm::abs(AABBA.HalfExtents.x) + glm::abs(AABBA.HalfExtents.y);
    float BTopRightX = PosB.x + glm::abs(AABBB.HalfExtents.x) - glm::abs(AABBB.HalfExtents.y);
    float BBottomRightX = PosB.x + glm::abs(AABBB.HalfExtents.x) + glm::abs(AABBB.HalfExtents.y);

    if (AAboveB == true)
    {
        //Finds the distance of the y value of the bottom right of A and the top right of the B
        penYA = ABottomRightY - BTopRightY;
    }
    else
    {
        //Finds the distance of the y value of the top right of the A and the bottom right of B

        penYB = ATopRightY - BBottomRightY;
    }
    if (ALeftOfB == true)
    {
        //Finds the distance of the x value of the bottom right of A and the top right of the B
        penXA = ABottomRightX - BTopRightX;
    }
    else
    {
        //Finds the distance of the x value of the top right of the A and the bottom right of B
        penXB = ATopRightX - BBottomRightX;
    }

    //if (PosA.x - glm::abs(AABBA.HalfExtents.x) < PosB.x + glm::abs(AABBB.HalfExtents.x)) // LEFT of A within RIGHT of B
    //{
    //    float ALeft = PosA.x - glm::abs(AABBA.HalfExtents.x);
    //    float BRight = PosB.x + glm::abs(AABBB.HalfExtents.x);

    //    float BPen = BRight - ALeft;
    //    //float BPen = ALeft - BRight;

    //    penXA = BPen;
    //}
    //if (PosA.x + glm::abs(AABBA.HalfExtents.x) > PosB.x - glm::abs(AABBB.HalfExtents.x)) // RIGHT of A within LEFT of B
    //{
    //    float ARight = PosA.x + glm::abs(AABBA.HalfExtents.x);
    //    float BLeft = PosB.x - glm::abs(AABBB.HalfExtents.x);

    //    float BPen = BLeft - ARight;
    //    //float BPen = ARight - BLeft;

    //    penXB = BPen;
    //}
    //if (PosA.y - glm::abs(AABBA.HalfExtents.y) < PosB.y + glm::abs(AABBB.HalfExtents.y)) // TOP of A within BOTTOM of B
    //{
    //    float ATop = PosA.y - glm::abs(AABBA.HalfExtents.y);
    //    float BBottom = PosB.y + glm::abs(AABBB.HalfExtents.y);

    //    float BPen = BBottom - ATop;
    //    //float BPen = ATop - BBottom;

    //    penYA = BPen;
    //}
    //if (PosA.y + glm::abs(AABBA.HalfExtents.y) > PosB.y - glm::abs(AABBB.HalfExtents.y)) // Bottom of A within Top of B
    //{
    //    float ABottom = PosA.y + glm::abs(AABBA.HalfExtents.y);
    //    float BTop = PosB.y - glm::abs(AABBB.HalfExtents.y);

    //    float BPen = BTop - ABottom;
    //    //float BPen = ABottom - BTop;

    //    penYB = BPen;
    //}

    penX = fminf(glm::abs(penXA), glm::abs(penXB));
    penY = fminf(glm::abs(penYA), glm::abs(penYB));

    Pen = fminf(glm::abs(penX), glm::abs(penY));

    // return the direction to correct along
    // if A is to the left of B
    if (penX == penXA)
    {
        //If a is above B
        if (penY == penYA)
        {
            //A is Above and to the Left of b
            //Bottom Right of A
            glm::vec2 BRA = glm::vec2(PosA.x + glm::abs(AABBA.HalfExtents.x), PosA.y + glm::abs(AABBA.HalfExtents.y));

            //The point of collision
            // If A's Bottom Right point's y is less than (higher) PosB.y
            if (BRA.y <= PosB.y)
            {
                BRA = glm::vec2(BRA.x, BRA.y - (BRA.y - PosB.y));
            }
            // If A's Bottom Right point's x is less than PosB.x
            else if (BRA.x <= PosB.x)
            {
                BRA = glm::vec2(BRA.x - (BRA.x - PosB.x), BRA.y);
            }
            return glm::normalize(PosB - BRA);
        }
        //If a is below B
        else if (penY == penYB)
        {
            //A is Below and to the Left of b
            //Top Right of A
            glm::vec2 TRA = glm::vec2(PosA.x + glm::abs(AABBA.HalfExtents.x), PosA.y - glm::abs(AABBA.HalfExtents.y));

            //The point of collision
            // If A's top right point's y is greater than (lower) PosB.y
            if (TRA.y >= PosB.y)
            {
                TRA = glm::vec2(TRA.x, TRA.y + (TRA.y - PosB.y));
            }
            // If A's top right point's x is less than PosB.x
            else if (TRA.x <= PosB.x)
            {
                TRA = glm::vec2(TRA.x - (TRA.x - PosB.x), TRA.y);
            }
            return glm::normalize(PosB - TRA);
        }
    }
    // if A is to the right of B
    else if(penX == penXB)
    {
        //If a is above B
        if (penY == penYA)
        {
            //A is Above and to the Right of b
            //Bottom Left of A
            glm::vec2 BLA = glm::vec2(PosA.x - glm::abs(AABBA.HalfExtents.x), PosA.y + glm::abs(AABBA.HalfExtents.y));

            //The point of collision
            // If A's Bottom Left point's y is less than (higher) PosB.y
            if (BLA.y <= PosB.y)
            {
                BLA = glm::vec2(BLA.x, BLA.y - (BLA.y - PosB.y));
            }
            // If A's Bottom left point's x is greater than PosB.x
            else if (BLA.x >= PosB.x)
            {
                BLA = glm::vec2(BLA.x + (BLA.x - PosB.x), BLA.y);
            }
            return glm::normalize(PosB - BLA);
        }
        //If a is below B
        else if (penY == penYB)
        {
            //A is Below and to the Right of b
            //Top Left of A
            glm::vec2 TLA = glm::vec2(PosA.x - glm::abs(AABBA.HalfExtents.x), PosA.y - glm::abs(AABBA.HalfExtents.y));

            //The point of collision
            // If A's top left point's y is greater than (lower) PosB.y
            if (TLA.y >= PosB.y)
            {
                TLA = glm::vec2(TLA.x, TLA.y + (TLA.y - PosB.y));
            }
            // If A's top left point's x is greater than PosB.x
            else if (TLA.x >= PosB.x)
            {
                TLA = glm::vec2(TLA.x + (TLA.x - PosB.x), TLA.y);
            }
            
            return glm::normalize(PosB - TLA);
        }
    }

    return glm::normalize(PosB - PosA);
}

glm::vec2 DepenetrateAABBAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen)
{
    return DepenetrateAABBAABB(PosA, ShapeA.AABBData, PosB, ShapeB.AABBData, Pen);
}

glm::vec2 DepenetrateCircleAABB(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const AABB& AABBB, float& Pen)
{
    float closestPointOnAABBX = glm::clamp(PosA.x, PosB.x - glm::abs(AABBB.HalfExtents.x), PosB.x + glm::abs(AABBB.HalfExtents.x));
    float closestPointOnAABBY = glm::clamp(PosA.y, PosB.y - glm::abs(AABBB.HalfExtents.y), PosB.y + glm::abs(AABBB.HalfExtents.y));
    glm::vec2 closestPointOnAABB = glm::vec2(closestPointOnAABBX, closestPointOnAABBY);

    Pen = CircleA.Radius - glm::distance(PosA, closestPointOnAABB);
    std::cout << " Penetration: " << Pen << std::endl;

    return glm::normalize(PosB - PosA);
}

glm::vec2 DepenetrateCircleAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen)
{
    return DepenetrateCircleAABB(PosA, ShapeA.CircleData, PosB, ShapeB.AABBData, Pen);
}

