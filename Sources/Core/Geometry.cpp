#include "Geometry.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace heavymach {
namespace {
float negateBits(float value) {
    return std::bit_cast<float>(std::bit_cast<uint32_t>(value) ^ 0x80000000u);
}
}
void Vector2::setXY(float newX, float newY) { x = newX; y = newY; }
void Vector2::setAngleLen(float a, float len) { x = std::cos(a) * len; y = len * std::sin(a); }
void Vector2::add(const Vector2& v) { x = x + v.x; y = y + v.y; }
void Vector2::sub(const Vector2& v) { x = x - v.x; y = y - v.y; }
void Vector2::mul(float v) { x = v * x; y = v * y; }
void Vector2::div(float v) { x = x / v; y = y / v; }
void Vector2::reverse() { x = negateBits(x); y = negateBits(y); }
float Vector2::dotProduct(const Vector2& v) const { return x * v.x + y * v.y; }
float Vector2::crossProduct(const Vector2& v) const { return x * v.y - y * v.x; }
float Vector2::squereLength() const { return x * x + y * y; }
float Vector2::length() const { return std::sqrt(squereLength()); }
float Vector2::angle() const { return static_cast<float>(std::atan2(static_cast<double>(y), static_cast<double>(x))); }
void Vector2::normalize() {
    const float len = length();
    if (len < std::bit_cast<float>(uint32_t{0x3727c5ac})) { x = 0; y = 0; }
    else { x = x / len; y = y / len; }
}
void Vector2::copyFrom(const Vector2& v) { x = v.x; y = v.y; }
ColVector::ColVector(const Vector2& a, const Vector2& b) { setOriginSecond(a, b); }
void ColVector::setOriginSecond(const Vector2& a, const Vector2& b) {
    origin.copyFrom(a);
    dir.copyFrom(b);
    dir.sub(a);
    length = dir.length();
    dir.div(length); // No zero-length guard in original 0x59a0.
    normal.setXY(dir.y, negateBits(dir.x));
}
std::optional<Vector2> ColVector::checkCollideColRadius(Vector2& p, float r) const {
    p.sub(origin);
    std::optional<Vector2> contact;
    const float along = dir.dotProduct(p);
    if (!(along > r + length) && !(along < negateBits(r))) {
        const float distance = normal.dotProduct(p);
        if (!(r <= std::fabs(distance))) {
            contact = p;
            contact->x = contact->x - distance * normal.x;
            contact->y = contact->y - distance * normal.y;
            contact->add(origin);
        }
    }
    p.add(origin); // Preserve original round-trip arithmetic, even on failure.
    return contact;
}
bool ColVector::checkCollideVel(Vector2& p, const Vector2& v) const {
    const Vector2 saved = p;
    p.sub(origin);
    if (!(v.dotProduct(normal) > 0)) {
        const float before = normal.dotProduct(p);
        p.add(v);
        const float after = normal.dotProduct(p);
        if (before >= 0 && after <= 0) {
            const float push = 0.5f - after;
            p.x = p.x + normal.x * push;
            p.y = p.y + push * normal.y;
            const float along = dir.dotProduct(p);
            if (!(along > 3.0f + length) && !(along < -3.0f)) {
                p.add(origin);
                return true;
            }
        }
    }
    p = saved;
    return false;
}
std::optional<Vector2> ColVector::checkCollideDirColRadius(Vector2& p, const Vector2& d, float r) const {
    const float along = dir.dotProduct(p);
    if (along > length || along < 0) return std::nullopt;
    const float toward = d.dotProduct(normal);
    if (toward > 0) return std::nullopt;
    const float distance = normal.dotProduct(p);
    if (std::fabs(distance) >= r) return std::nullopt;
    const float step = (r - distance) / toward;
    p.x = p.x + step * d.x;
    p.y = p.y + step * d.y;
    Vector2 contact = p;
    contact.x = contact.x - r * normal.x;
    contact.y = contact.y - r * normal.y;
    contact.add(origin);
    return contact;
}
std::optional<Vector2> ColVector::checkCollideVelColRadius(const Vector2& p, const Vector2& v, float r) const {
    if (!(r > 0) || !std::isfinite(r))
        throw std::invalid_argument("ColVector sweep requires a finite positive radius");
    Vector2 relative = p, direction = v;
    relative.sub(origin);
    float remaining = direction.length();
    direction.div(remaining);
    for (;;) {
        if (!(r < remaining)) {
            relative.copyFrom(p);
            relative.sub(origin);
            relative.add(v);
            return checkCollideDirColRadius(relative, direction, r);
        }
        relative.x = relative.x + r * direction.x;
        relative.y = relative.y + r * direction.y;
        auto contact = checkCollideDirColRadius(relative, direction, r);
        const float nextRemaining = remaining - r;
        // Explicit domain failure rather than silently hanging on float stagnation.
        if (!contact && nextRemaining == remaining)
            throw std::domain_error("ColVector sweep does not progress at this float magnitude");
        remaining = nextRemaining;
        if (contact) return contact;
    }
}
}
