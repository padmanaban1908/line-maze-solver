# ⚡ Hardware Schematics & Circuit Architecture

Detailed hardware pinouts, power distribution, and component choices for the Line Maze Solver Robot.

---

## 📌 Pinout Mapping (Arduino Uno / Nano Example)

| Component | Pin Function | Arduino Pin | Notes |
|---|---|---|---|
| **Motor Driver (TB6612FNG)** | Left Motor PWM | D5 | Hardware Timer PWM |
| | Left Motor IN1 | D7 | Direction Control |
| | Left Motor IN2 | D8 | Direction Control |
| | Right Motor PWM | D6 | Hardware Timer PWM |
| | Right Motor IN1 | D9 | Direction Control |
| | Right Motor IN2 | D10 | Direction Control |
| | Standby (STBY) | 5V or D4 | Pulled HIGH to enable driver |
| **Sensor Array (5x IR)** | Left Outer (L2) | A0 | Analog or Digital Read |
| | Left Inner (L1) | A1 | Analog or Digital Read |
| | Center (C) | A2 | Line Centering Reference |
| | Right Inner (R1) | A3 | Analog or Digital Read |
| | Right Outer (R2) | A4 | Analog or Digital Read |
| **Control UI** | Push Button (Mode) | D2 | Interrupt pin (INPUT_PULLUP) |
| | Status Indicator | D13 | Onboard LED (Explore vs Speed Run) |

---

## 🔋 Power Delivery Architecture

- **Primary Battery**: 2S Li-Po Battery (7.4V nominal, 800–1200 mAh, 25C).
- **Motor Power (VM)**: 7.4V direct from battery to motor driver VM pin.
- **Logic Power (VCC)**: 5.0V regulated via a high-efficiency buck converter (LM2596 / MP1584) feeding Arduino 5V pin and sensor array VCC.
- **Ground (GND)**: Common ground shared across battery, buck converter, microcontroller, sensor array, and motor driver.

---

## 🏎️ Recommended Motors & Chassis

- **Motors**: N20 Micro Metal Gearmotors (6V, 300 to 600 RPM).
- **Wheels**: Pololu 32mm or 42mm high-grip silicone wheels.
- **Caster**: Front/rear nylon ball transfer unit or Teflon skid.
- **Sensor Standoff**: 3 mm to 5 mm above the arena surface for optimal infrared reflectivity.

