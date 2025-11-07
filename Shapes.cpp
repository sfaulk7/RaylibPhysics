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

    if (PosA.x - glm::abs(AABBA.HalfExtents.x) < PosB.x + glm::abs(AABBB.HalfExtents.x)) // LEFT of A within RIGHT of B
    {
        float ALeft = PosA.x - glm::abs(AABBA.HalfExtents.x);
        float BRight = PosB.x + glm::abs(AABBB.HalfExtents.x);

        //float BPen = BRight - ALeft;
        float BPen = ALeft - BRight;

        penXA = BPen;
    }
    if (PosA.x + glm::abs(AABBA.HalfExtents.x) > PosB.x - glm::abs(AABBB.HalfExtents.x)) // RIGHT of A within LEFT of B
    {
        float ARight = PosA.x + glm::abs(AABBA.HalfExtents.x);
        float BLeft = PosB.x - glm::abs(AABBB.HalfExtents.x);

        //float BPen = BLeft - ARight;
        float BPen = ARight - BLeft;

        penXB = BPen;
    }
    if (PosA.y - glm::abs(AABBA.HalfExtents.y) < PosB.y + glm::abs(AABBB.HalfExtents.y)) // TOP of A within BOTTOM of B
    {
        float ATop = PosA.y - glm::abs(AABBA.HalfExtents.y);
        float BBottom = PosB.y + glm::abs(AABBB.HalfExtents.y);

        //float BPen = BBottom - ATop;
        float BPen = ATop - BBottom;

        penYA = BPen;
    }
    if (PosA.y + glm::abs(AABBA.HalfExtents.y) > PosB.y - glm::abs(AABBB.HalfExtents.y)) // Bottom of A within Top of B
    {
        float ABottom = PosA.y + glm::abs(AABBA.HalfExtents.y);
        float BTop = PosB.y - glm::abs(AABBB.HalfExtents.y);

        //float BPen = BTop - ABottom;
        float BPen = ABottom - BTop;

        penYB = BPen;
    }

    penX = fminf(glm::abs(penXA), glm::abs(penXB));
    penY = fminf(glm::abs(penYA), glm::abs(penYB));

    Pen = fminf(glm::abs(penX), glm::abs(penY));
    // return the direction to correct along
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

