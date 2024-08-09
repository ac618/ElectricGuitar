# STM32F746G-DISCO LCD Plot

This project implements a data visualization feature on the STM32F746G-DISCO board, similar to MATLAB's `plot()` function, using the LCD BSP library. The project is generated with CubeMX and built using CMake.

## Current Progress

- **Static Plotting:** The project currently supports only static plots.
- **Future Enhancements:** Realtime plotting functionality may be added in future updates.

## How to Run

### Using the STM32 VSCode Extension

1. Open the STM32 Extension and select "Import CMake Project" to generate the `.vscode/` directory.
2. In the terminal, navigate to **Terminal -> Run Task -> Build + Flash** to compile and flash the firmware onto the board.
