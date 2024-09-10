# NUCLEO-F429ZI DAC Wave Generator

This project generate a sine wave using DAC of NUCLEO-F429ZI board. The sine wave is generated on PA4 pin of the board. The parameters of the sine wave (frequency, sampling rate, amplitude) are defined in user private defines.

## How to Run

### Using the STM32 VSCode Extension

1. Open the STM32 Extension and select "Import CMake Project" to generate the `.vscode/` directory.
2. In the terminal, navigate to **Terminal -> Run Task -> Build + Flash** to compile and flash the firmware onto the board.

## Parameters
In the private define code, the parameters of the sine wave can be configured, including `SAMPLING_RATE`, `SINE_FREQUENCY` and `AMPLITUDE`.
The `DAC_BUFFER_SIZE` is calculated by the parameters.
The `PRESCALED_FREQUENCY` is the frequency of timer2 / (PSC + 1), which is configured in CubeMx.
```C
/* USER CODE BEGIN PD */
#define SAMPLING_RATE 100000
#define SINE_FREQUENCY 500
#define AMPLITUDE 0.5 // volts in the range of [0, 1.65]
#define DAC_BUFFER_SIZE (SAMPLING_RATE/SINE_FREQUENCY) // Do not modify
#define PRESCALED_FREQUENCY 1000000 // frequency of TIM2 / (PSC + 1)
/* USER CODE END PD */
```