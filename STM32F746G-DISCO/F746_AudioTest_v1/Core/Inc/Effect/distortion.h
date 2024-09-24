#ifndef __DISTORTION_H
#define __DISTORTION_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    void DistortionProcess(uint8_t *data, float gain, int16_t threshold, uint32_t num_samples);

#ifdef __cplusplus
}
#endif

#endif /* __DISTORTION_H */