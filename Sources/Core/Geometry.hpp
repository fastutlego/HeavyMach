#pragma once
#include <optional>

namespace heavymach {
// Source reconstruction of Vector2; methods retain the original selector names.
struct Vector2 {
    float x = 0, y = 0;
    void setXY(float newX, float newY);
    void setAngleLen(float angle, float len);
    void add(const Vector2& value);
    void sub(const Vector2& value);
    void mul(float value);
    void div(float value);
    void reverse();
    float dotProduct(const Vector2& value) const;
    float crossProduct(const Vector2& value) const;
    float length() const;
    float squereLength() const; // Original selector spelling.
    float angle() const;
    void normalize();
    void copyFrom(const Vector2& value);
};

class ColVector {
public:
    ColVector() = default;
    ColVector(const Vector2& origin, const Vector2& second);
    void setOriginSecond(const Vector2& origin, const Vector2& second);
    void setPrev(ColVector* value) { prev = value; }
    void setNext(ColVector* value) { next = value; }
    // checkCollide:ColRadius: temporarily subtracts/adds origin to point.
    std::optional<Vector2> checkCollideColRadius(Vector2& point, float radius) const;
    // checkCollide:Vel: writes the corrected point only on success.
    bool checkCollideVel(Vector2& point, const Vector2& velocity) const;
    // checkCollide:Dir:ColRadius: expects point RELATIVE to origin;
    // modifies that relative point on success, returns a world-space contact.
    std::optional<Vector2> checkCollideDirColRadius(Vector2& relativePoint,
                                                   const Vector2& direction, float radius) const;
    // checkCollide:Vel:ColRadius: does not mutate either caller vector.
    // Finite positive radius required: original loop has no invalid-input guard.
    std::optional<Vector2> checkCollideVelColRadius(const Vector2& point,
                                                   const Vector2& velocity, float radius) const;
    Vector2 origin{}, dir{}, normal{};
    float length = 0;
    ColVector* prev = nullptr;
    ColVector* next = nullptr;
};
}
