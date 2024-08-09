# STM32F746G-DISCO ADC Voltmeter

This project is a simple ADC voltmeter for the STM32F746G-DISCO board. It reads the voltage on the PA0 (A0) pin and displays both the raw ADC value and the corresponding voltage on the LCD screen.

## Limitations

- The PA0 pin can only measure voltages in the range of 0V to 3.3V.
- Exceeding 3.3V on the PA0 pin may result in damage to the microcontroller.

## How to Run

### Using the STM32 VSCode Extension

1. Open the STM32 Extension and select "Import CMake Project" to generate the `.vscode/` directory.
2. In the terminal, navigate to **Terminal -> Run Task -> Build + Flash** to compile and flash the firmware onto the board.
