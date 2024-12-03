#ifndef __WAHWAH_H
#define __WAHWAH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "main.h"

    void WahWah_Init(void);
    void WahWahProcess(uint8_t *data, float center_freq, float lfo_freq, float lfo_depth, uint32_t num_samples);

#ifdef __cplusplus
}
#endif

#endif /* __WAHWAH_H */