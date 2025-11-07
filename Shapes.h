#pragma once

#include <cstdint>        // for uint8_t

#include "glm/vec2.hpp"   // for glm::vec2

struct Circle
{
    float Radius = 10.0f;
};

struct AABB
{
    glm::vec2 HalfExtents;
};

enum class ShapeType : uint8_t
{
    NONE   = 0,      // 0000 0000
    CIRCLE = 1 << 0, // 0000 0001
    AABB   = 1 << 1  // 0000 0010
};

struct Shape
{
    // an enum identifying
    ShapeType Type;

    // add new types of shapes to this anonymous union
    union
    {
        Circle CircleData;
        AABB AABBData;
    };
};

// compares two circles and returns true if they are colliding
bool CheckCircleCircle(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const Circle& CircleB);

// Shape wrapper for CIRCLE-CIRCLE collisions
bool CheckCircleCircle(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB);

// compares two AABB and returns true in they are colliding
bool CheckAABBAABB(const glm::vec2& PosA, const AABB& AABBA, const glm::vec2& PosB, const AABB& AABBB);

// Shape wrapper for AABB-AABB collisions
bool CheckAABBAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB);

// compares a Circle and an AABB and returns true in they are colliding
bool CheckCircleAABB(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const AABB& AABBB);

// Shape wrapper for CIRCLE-AABB collisions
bool CheckCircleAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB);


// returns minimum translation direction (MTD)
// penetration depth written to 'pen'
glm::vec2 DepenetrateCircleCircle(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const Circle& CircleB, float& Pen);

// Wrapper for compatibility with Shapes (calls the above)
// returns minimum translation direction (MTD)
// penetration depth written to 'pen'
glm::vec2 DepenetrateCircleCircle(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen);

// returns minimum translation direction (MTD)
// penetration depth written to 'pen'
glm::vec2 DepenetrateAABBAABB(const glm::vec2& PosA, const AABB& AABBA, const glm::vec2& PosB, const AABB& AABBB, float& Pen);

// Wrapper for compatibility with Shapes (calls the above)
// returns minimum translation direction (MTD)
// penetration depth written to 'pen'
glm::vec2 DepenetrateAABBAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen);

// returns minimum translation direction (MTD)
// penetration depth written to 'pen'
glm::vec2 DepenetrateCircleAABB(const glm::vec2& PosA, const Circle& CircleA, const glm::vec2& PosB, const AABB& CircleB, float& Pen);

// Wrapper for compatibility with Shapes (calls the above)
// returns minimum translation direction (MTD)
// penetration depth written to 'pen'
glm::vec2 DepenetrateCircleAABB(const glm::vec2& PosA, const Shape& ShapeA, const glm::vec2& PosB, const Shape& ShapeB, float& Pen);