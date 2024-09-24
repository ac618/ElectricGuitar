#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    void DelayProcess(uint8_t *data, float gain, uint16_t delaySamples, uint32_t num_samples);

#ifdef __cplusplus
}
#endif

#endif /* __DELAY_H */