#include "wahwah.h"

#define _USE_MATH_DEFINES
#include "math.h"


// bandpass filter coeff's.
static float bp_a0, bp_a1, bp_a2, bp_b0, bp_b1, bp_b2;

// bandpass previous samples.
static float bp_x1, bp_x2, bp_y1, bp_y2;

static float currentCutoff = 440;
static float qFactor = 2;

static void new_bandpass()
{
    float omega = (float)(2 * (float)(M_PI)*currentCutoff) / (float)(AUDIO_FREQ);
    float cosomega = cosf(omega);
    float alpha = sinf(omega) / (2 * qFactor);

    // Compute raw coefficients
    float b0 = alpha;
    float b1 = 0;
    float b2 = -alpha;
    float a0 = 1 + alpha;
    float a1 = -2 * cosomega;
    float a2 = 1 - alpha;

    // Normalize coefficients to ensure peak gain = 1
    bp_b0 = b0 / a0;
    bp_b1 = b1 / a0;
    bp_b2 = b2 / a0;
    bp_a1 = a1 / a0;
    bp_a2 = a2 / a0;
}

static int16_t apply_bandpass(int16_t inSample)
{
    float x0 = (float)inSample;

    float result =
        bp_b0 * x0 +
        bp_b1 * bp_x1 +
        bp_b2 * bp_x2 -
        bp_a1 * bp_y1 -
        bp_a2 * bp_y2;

    // Shift samples to simulate delay
    bp_x2 = bp_x1;
    bp_x1 = x0;
    bp_y2 = bp_y1;
    bp_y1 = result;

    return (int16_t)result;
}

void WahWah_Init(void)
{
    bp_x1 = bp_x2 = bp_y1 = bp_y2 = 0;
    new_bandpass();
}

void WahWahProcess(uint8_t *data, float center_freq, float lfo_freq, float lfo_depth, uint32_t num_samples)
{
    static float phase = 0;
    static int counter = 0;
    int update_rate = 1;

    for (uint32_t i = 0; i < num_samples; i = i + 2)
    {
        uint8_t high_byte = data[i + 1];
        uint8_t low_byte = data[i];
        int16_t sample = (high_byte << 8) | low_byte;

        // update phase
        phase = fmodf((phase + lfo_freq / AUDIO_FREQ), 1);

        // get smoother sine LFO sample instead of triangle
        float lfo_sample = phase < 0.5 ? phase * 4 - 1 : 3 - 4 * phase;
        // Modulate the cutoff frequency more smoothly
        if (counter % update_rate == 0)
        {
            currentCutoff = (float)((lfo_sample * lfo_depth * center_freq) + center_freq);
            new_bandpass();
        }

        // apply bandpass filter to current sample
        sample = apply_bandpass(sample);

        data[i + 1] = (sample >> 8) & 0xFF;
        data[i] = sample & 0xFF;
    }
}