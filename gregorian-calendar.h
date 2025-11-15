#include <stdint.h>

static uint32_t gregorian_to_jdn(uint32_t gy, uint32_t gm, uint32_t gd)
{
    if (gy < 3)
    {
        gy--;
        gm += 12;
    }
    return 356032 + 365 * gy + gy / 4 - gy / 100 + gy / 400 + (153 * (gm - 3) + 2) / 5 + gd - 1 - 305;
}
