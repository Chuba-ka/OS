#include "../include/ECalculator.hpp"

class ESeries : public ECalculator
{
public:
    float calculate(int x) const override
    {
        if (x < 0)
            return 0.0f;
        float sum = 1.0f, term = 1.0f;
        for (int n = 1; n <= x; ++n)
        {
            term /= n;
            sum += term;
        }
        return sum;
    }
    const char *name() const override { return "Taylor series"; }
};
extern "C" ECalculator *create_e() { return new ESeries(); }
extern "C" void destroy_e(ECalculator *p) { delete p; }