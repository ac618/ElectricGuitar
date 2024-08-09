# NUCLEO-F429ZI DAC Wave Generator

This project generate a 400 Hz sine wave in the range of [0, 2] V with 10000 Hz sampling rate using DAC of NUCLEO-F429ZI board. The sine wave is generated on PA4 pin of the board.

## How to Run

### Using the STM32 VSCode Extension

1. Open the STM32 Extension and select "Import CMake Project" to generate the `.vscode/` directory.
2. In the terminal, navigate to **Terminal -> Run Task -> Build + Flash** to compile and flash the firmware onto the board.
