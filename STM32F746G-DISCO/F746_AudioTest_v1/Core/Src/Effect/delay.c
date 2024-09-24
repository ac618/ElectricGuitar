#include "delay.h"

void DelayProcess(uint8_t *data, float gain, uint16_t delaySamples, uint32_t num_samples)
{
    for (uint32_t i = 0; i < num_samples; i = i + 2)
    {
        uint8_t high_byte = data[i + 1];
        uint8_t low_byte = data[i];
        int16_t sample = (high_byte << 8) | low_byte;

        uint8_t prev_high_byte = data[(i + num_samples - delaySamples + 1) % num_samples];
        uint8_t prev_low_byte = data[(i + num_samples - delaySamples) % num_samples];
        int16_t prev_sample = (prev_high_byte << 8) | prev_low_byte;

        prev_sample = (int16_t)(prev_sample * gain);
        int16_t combinedData = sample + prev_sample;
        data[i + 1] = (combinedData >> 8) & 0xFF;
        data[i] = combinedData & 0xFF;
    }
}