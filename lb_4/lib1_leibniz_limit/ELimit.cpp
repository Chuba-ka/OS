#include "../include/ECalculator.hpp"
#include <cmath>

class ELimit : public ECalculator
{
public:
    float calculate(int x) const override
    {
        return (x <= 0) ? 1.0f : powf(1.0f + 1.0f / x, x);
    }
    const char *name() const override { return "Limit (1+1/x)^x"; }
};

extern "C" ECalculator *create_e() { return new ELimit(); }
extern "C" void destroy_e(ECalculator *p) { delete p; }