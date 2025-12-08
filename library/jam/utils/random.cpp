#include "random.hpp"

namespace jam
{
    static std::random_device s_randomDevice;
    static std::mt19937_64 s_Engine(s_randomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

    int GetRandomInt(int min, int max)
    {
        std::uniform_int_distribution<int> distro(min, max);
        return distro(s_Engine);
    }
    uint32_t GetRandomUint32(uint32_t min, uint32_t max)
    {
        std::uniform_int_distribution<uint32_t> distro(min, max);
        return distro(s_Engine);
    }
    float GetRandomFloat(float min, float max)
    {
        std::uniform_real_distribution<> distro(min, max);
        return distro(s_Engine);
    }
    uint64_t GetRandomUint64()
    {
        return s_UniformDistribution(s_Engine);
    }
}
