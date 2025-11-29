#include "../include/PiCalculator.hpp"

class PiLeibniz : public PiCalculator
{
public:
    float calculate(int K) const override
    {
        if (K <= 0)
            return 0.0f;
        float sum = 0.0f;
        for (int i = 0; i < K; ++i)
            sum += (i & 1) ? -1.0f : 1.0f / (2 * i + 1);
        return 4.0f * sum;
    }
    const char *name() const override { return "Leibniz series"; }
};

extern "C" PiCalculator *create_pi() { return new PiLeibniz(); }
extern "C" void destroy_pi(PiCalculator *p) { delete p; }