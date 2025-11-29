#pragma once
class ECalculator
{
public:
    virtual ~ECalculator() = default;
    virtual float calculate(int x) const = 0;
    virtual const char *name() const = 0;
};