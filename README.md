ATmega328p door lock with keypad

This project is for the class ENGC50 - Microprocessors
The ideia was to build a lock that can open a door or trigger an alarm using only AVR.
For that I basically build some libs but they are optimazed for my usage, therefore they are not properly complete.

The simulation file for SimulIDE and the built hex can be found in the folder "Simulação". And a bundled ZIP with all files is available.

### Abilities used in the project:
AVR328:
- Programing and usage (r/w) of IO pins
- Programing and usage of the USART port
- Interrupts (timer + serial)
- Internal time counter
- Log via serial comunication of the usage of the lock
- EEPROM save/load data



