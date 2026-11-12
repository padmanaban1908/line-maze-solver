# 🎯 IR Sensor Array Calibration Guide

Accurate sensor calibration is essential for reliable junction detection and smooth PID line following.

---

## 🔍 How Reflectance Sensors Work
- An infrared LED shines light downwards.
- A phototransistor measures reflected light intensity.
- **White Surface**: High reflection $\implies$ low resistance / high analog reading.
- **Black Line**: Low reflection (IR absorbed) $\implies$ high resistance / low analog reading.

---

## 🧪 Calibration Procedure

1. **Auto-Calibration Spin on Startup**:
   - In `setup()`, rotate the robot left and right by ~45 degrees for 3 seconds.
   - Record the minimum value (`minVal[i]`) and maximum value (`maxVal[i]`) seen by each sensor pin:
     ```cpp
     for (int i = 0; i < NUM_SENSORS; i++) {
         int val = analogRead(SENSOR_PINS[i]);
         if (val < minVal[i]) minVal[i] = val;
         if (val > maxVal[i]) maxVal[i] = val;
     }
     ```
2. **Threshold Calculation**:
   - Compute the midpoint threshold for each sensor:
     $$\text{Threshold}_i = \frac{\text{minVal}_i + \text{maxVal}_i}{2}$$
3. **Normalized Read**:
   - Any value above $\text{Threshold}_i$ is treated as black line (`1`), otherwise white surface (`0`).

---

## 🛠️ Common Troubleshooting

| Symptom | Root Cause | Fix |
|---|---|---|
| Robot wobbles violently on straight lines | $K_p$ gain too high or $K_d$ too low | Lower $K_p$, slightly raise $K_d$ |
| Robot overshoots 90° turns | Speed too high entering junction | Reduce `turnSpeed` and add junction debounce |
| Robot doesn't see dead-ends | Ambient sunlight or uneven sensor height | Shield sensor array from direct ambient light, mount sensors 3–5mm above surface |

