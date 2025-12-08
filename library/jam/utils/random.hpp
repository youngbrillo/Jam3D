#pragma once
#include <random>

namespace jam
{
    int GetRandomInt(int min, int max);
    uint32_t GetRandomUint32(uint32_t min, uint32_t max);
    float GetRandomFloat(float min, float max);
    uint64_t GetRandomUint64();
} // namespace jam::utilities
