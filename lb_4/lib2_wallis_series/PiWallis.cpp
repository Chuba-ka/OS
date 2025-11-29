#include "../include/PiCalculator.hpp"

class PiWallis : public PiCalculator
{
public:
    float calculate(int K) const override
    {
        if (K <= 0)
            return 0.0f;
        float p = 1.0f;
        for (int i = 1; i <= K; ++i)
            p *= (4.0f * i * i) / (4.0f * i * i - 1.0f);
        return 2.0f * p;
    }
    const char *name() const override { return "Wallis product"; }
};
extern "C" PiCalculator *create_pi() { return new PiWallis(); }
extern "C" void destroy_pi(PiCalculator *p) { delete p; }