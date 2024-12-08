#ifndef HELPERS_H
#define HELPERS_H

#include "Transform.h"
#include <cmath>

// Простий трансформатор для множення
class Multiply
{
public:
    explicit Multiply(float factor) : m_factor(factor) {}
    Multiply() = default;

    void init (float factor) {
        m_factor = factor;
    }

    inline constexpr float apply(float value) const {
        return value * m_factor;
    }

private:
    float m_factor;
};

// Простий трансформатор для додавання
class Add {
public:
    explicit Add(float increment) : m_increment(increment) {}
    Add() = default;

    void init (float increment) {
        m_increment = increment;
    }

    inline constexpr float apply(float value) const {
        return value + m_increment;
    }

private:
    float m_increment;
};

// Трансформатор для квадратного кореня
class Sqrt {
public:
    Sqrt() = default;
    inline float apply(float value) const {
        return std::sqrt(value);
    }
};


#endif // HELPERS_H
