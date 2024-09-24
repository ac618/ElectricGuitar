#include "distortion.h"

void DistortionProcess(uint8_t *data, float gain, int16_t threshold, uint32_t num_samples)
{
    for (uint32_t i = 0; i < num_samples; i = i + 2)
    {
        uint8_t high_byte = data[i + 1];
        uint8_t low_byte = data[i];
        int16_t sample = (high_byte << 8) | low_byte;

        sample = (int16_t)(sample * gain); // apply gain
        if (sample > threshold)
        {
            sample = threshold;
        }
        else if (sample < -threshold)
        {
            sample = -threshold;
        }
        data[i + 1] = (sample >> 8) & 0xFF;
        data[i] = sample & 0xFF;
    }
}