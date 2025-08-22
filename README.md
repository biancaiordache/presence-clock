# ESP32 Presence Clock ⏱️

A simple timer/clock project built on the ESP32 and a TM1637 4-digit display.

## Features
- Displays MM:SS or HH:MM
- Counts upward from 00:00
- Start/Pause/Reset via Serial (BLE planned)
- Expandable: BLE control, 3D-printed enclosure, daily reset

## Hardware
- ESP32 DevKit v1
- TM1637 4-digit LED display
- Breadboard + jumper wires

## Wiring
| TM1637 Pin | ESP32 Pin |
|------------|-----------|
| VCC        | 5V (or 3V3 if module supports it) |
| GND        | GND |
| DIO        | GPIO 19 |
| CLK        | GPIO 18 |

## Roadmap
- [x] Basic timer
- [x] Serial start/pause/reset
- [ ] BLE control
- [ ] Enclosure
- [ ] Auto daily reset
