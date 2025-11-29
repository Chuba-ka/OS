#pragma once
class PiCalculator
{
public:
    virtual ~PiCalculator() = default;
    virtual float calculate(int K) const = 0;
    virtual const char *name() const = 0;
};