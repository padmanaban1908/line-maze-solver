# 💡 Project Ideas & Architecture: Line Maze Solver

This document serves as the central ideas notebook and technical architecture guide for the **Line Maze Solver Robot**. Whenever new ideas, hardware changes, or algorithmic improvements are developed, they are recorded and updated here.

---

## 🎯 1. Project Objectives

1. **Autonomous Maze Solving**:
   - Solve simple and complex 2D line mazes (black line on white background or white line on black background).
   - Handle 90° turns, T-junctions, cross-intersections, dead ends, and looped mazes.
2. **Dual-Run Strategy**:
   - **Run 1 (Exploration)**: Navigate the maze using the Left-Hand-on-Wall (LSRB) rule while recording every decision into memory. When dead ends are reached, reduce and eliminate them in real time.
   - **Run 2 (Speed Run)**: Read the optimized shortest path from EEPROM/RAM and navigate through the maze at maximum acceleration without exploring dead ends.
3. **Robust Line Following**:
   - Closed-loop PID control to maintain steady centering on straightaways and curves.

---

## 🛠️ 2. Hardware Architecture & Bill of Materials (BOM)

### Microcontroller Options
- **Option A (Beginner / Classic)**: Arduino Uno / Nano (ATmega328P) - Simple, 5V logic, easy pinouts.
- **Option B (Recommended / Fast)**: ESP32 / Raspberry Pi Pico / STM32 (BluePill) - High clock speed (160-240 MHz), ample RAM for large maze graphs, dual cores for motor PID + path computing.

### Sensor Array
- **Sensor Type**: Pololu QTR-8A / QTR-8RC (8-channel reflectance array) or 5-channel TCRT5000 array.
- **Mounting**: Front cantilever, 3–5 mm above the surface.
- **Sensor Weighting**:
  $$\text{Position} = \frac{\sum (i \times w_i \times S_i)}{\sum S_i}$$
  where $S_i$ is normalized reflectance reading, and $w_i$ are coordinate weights (e.g. $-3, -2, -1, 0, 1, 2, 3$).

### Motor Driver & Actuators
- **Motors**: 2x Micro Metal Gearmotors (6V, 300 to 600 RPM with magnetic encoders).
- **Motor Driver**: TB6612FNG (Dual H-Bridge, high efficiency MOSFET, cooler and faster response than legacy L298N).
- **Chassis**: Differential drive with a low-friction front/rear ball caster.
- **Power**: 2S LiPo battery (7.4V, 800–1200 mAh) with a 5V buck converter (LM2596 or MP1584).

---

## 🧠 3. Core Algorithms

### A. PID Control for Line Following
$$\text{Error} = \text{Target Position} - \text{Current Position}$$
$$P = K_p \times \text{Error}$$
$$I = K_i \times \sum \text{Error} \times \Delta t$$
$$D = K_d \times \frac{\text{Error} - \text{Previous Error}}{\Delta t}$$
$$\text{Correction} = P + I + D$$

$$\text{Speed}_{\text{Left}} = \text{BaseSpeed} + \text{Correction}$$
$$\text{Speed}_{\text{Right}} = \text{BaseSpeed} - \text{Correction}$$

### B. Junction Classification
At each time step, read the sensor pattern:
- **`00000`**: Dead end (all white) $\to$ Initiate 180° turn (`B`).
- **`11111`**: Cross intersection (`+`) or Goal (`E`) $\to$ Drive forward slightly to verify if line continues.
- **`11100`**: Left turn or T-junction $\to$ Turn left (`L`).
- **`00111`**: Right turn or Straight/Right $\to$ Check forward sensor; if straight available and LHS rule is used, go straight or right according to priority.

### C. Path Simplification Engine (LSRB Rules)
Whenever a sequence ends with `B` followed by another turn, replace the last 3 turns with the equivalent single turn:

```c
void simplifyPath() {
    if (pathLength < 3 || path[pathLength - 2] != 'B') return;

    int totalAngle = 0;
    int i;
    for (i = 1; i <= 3; i++) {
        switch (path[pathLength - i]) {
            case 'R': totalAngle += 90; break;
            case 'L': totalAngle += 270; break;
            case 'B': totalAngle += 180; break;
            case 'S': totalAngle += 0; break;
        }
    }
    totalAngle = totalAngle % 360;

    switch (totalAngle) {
        case 0:   path[pathLength - 3] = 'S'; break;
        case 90:  path[pathLength - 3] = 'R'; break;
        case 180: path[pathLength - 3] = 'B'; break;
        case 270: path[pathLength - 3] = 'L'; break;
    }
    pathLength -= 2;
}
```

---

## 🔮 4. Future Upgrades & Brainstorming Ideas

1. **Loop Detection & Trémaux's Algorithm**:
   - LSRB fails if the maze contains closed loops or islands.
   - Upgrade idea: Implement Trémaux's algorithm with junction coordinate odometry (wheel encoders + IMU MPU6050) to detect revisit nodes.
2. **Diagonal & Curved Line Optimization**:
   - Track wheel encoder pulses to reconstruct a vector map of the maze in 2D Euclidean space.
   - Calculate cutting corners or smooth transitions for 90-degree turns during Run 2.
3. **Telemetry & Live App (ESP32 Bluetooth/Wi-Fi WebSockets)**:
   - Stream sensor readings, battery level, current junction type, and path string to a mobile/web dashboard.
   - Live calibration interface to tune $K_p, K_i, K_d$ constants wirelessly.
4. **OLED Display & Pushbutton Mode Selector**:
   - 0.96-inch I2C OLED display showing:
     - Sensor calibration bar charts
     - Mode: `CALIBRATE` $\to$ `RUN 1 (EXPLORE)` $\to$ `RUN 2 (SPEED RUN)`
     - Path length & execution time.
