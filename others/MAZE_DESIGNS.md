# 🏆 Official Competition Line Maze Specifications & Winning Strategies

This document details the standard arena specifications, junction design rules, and scoring formulas used in national and international robotics competitions (e.g., IEEE Robotics, Robofest, Techfest, All Japan Line Maze).

---

## 📐 1. Official Arena Dimensions & Standards

| Parameter | Competition Specification | Engineering Purpose |
|---|---|---|
| **Arena Size** | $2.4\text{ m} \times 2.4\text{ m}$ ($8\text{ ft} \times 8\text{ ft}$) or $3\text{ m} \times 3\text{ m}$ | Standard modular wood/flex sheet size |
| **Line Width** | Exactly $19\text{ mm}$ ($\frac{3}{4}\text{ inch}$) | Matches standard commercial electrical tape width |
| **Line Material** | Matte Black Vinyl / PVC Tape (Scotch 33+ or Nitto) | High IR absorption, non-reflective |
| **Surface Material** | Matte White Flex Banner (Backside) or Polyurethane coated wood | High IR reflectivity, zero glare from overhead lighting |
| **Grid Unit / Pitch** | $15\text{ cm}$ to $25\text{ cm}$ between parallel tracks | Prevents sensor cross-detection of adjacent lines |
| **Start Box** | $30\text{ cm} \times 30\text{ cm}$ clear square | Alignment zone for starting position |
| **Finish Marker** | $10\text{ cm} \times 10\text{ cm}$ solid black square or full crossbar | Simultaneous detection on all sensors for $>100\text{ ms}$ |

---

## ⚠️ 2. The 5 Classic Competition Traps Organizers Build

Competition maze designers deliberately construct specific patterns to eliminate basic robots:

```
                  [Goal 🏁]
                     │
         ────────────┼──────────── (Trap 1: 4-Way Cross before Goal)
                     │
             ┌───────┴───────┐
             │               │
      (Dead End 200cm)       │     (Trap 2: High-Speed False Straight)
             │               │
             └───────┬───────┘
                     │
             ┌───────┴───────┐
             │               │
             │       ┌───────┘     (Trap 3: Rapid Double 90° Chicanes)
             │       │
             └───────┴───────┐
                             │
                          [Start]
```

### Trap 1: The "False Straight" (Dead End on Straightaway)
- **Design**: A long straight line pointing directly toward the goal that terminates into a dead end just 10 cm before the finish.
- **Trap**: Robots running too fast on straightaways overshoot the dead end and derail.
- **Countermeasure**: Dynamic braking in PID firmware when sensors suddenly read all white (`0 0 0 0 0`).

### Trap 2: Rapid 90° Zig-Zags (Chicanes)
- **Design**: Two 90° turns spaced only 10–15 cm apart.
- **Trap**: Chassis inertia causes the robot to oscillate or overshoot the second turn.
- **Countermeasure**: Lower wheel mass, silicone tires, and high $K_d$ derivative damping.

### Trap 3: The 4-Way Cross (`+`) Ambiguity
- **Design**: A cross intersection with a dead end on the left branch, straight leading to goal, right leading to another loop.
- **Trap**: Inching distance must be tuned to within $\pm 5\text{ mm}$, otherwise the robot misidentifies the intersection as a T-junction or turns off-axis.

---

## ⏱️ 3. Competition Rules & Scoring Formula

Most competitions grant a total time slot of **3 to 5 minutes** per team with 2 official runs:

$$\text{Final Score} = \text{Run 2 Time (seconds)} + \text{Penalties}$$

### Penalty Deductions:
- **Derailment / Hand Touch**: $+5\text{ seconds}$ to $+10\text{ seconds}$ penalty per touch, with the robot reset to the start.
- **Max Exploration Time**: If Run 1 exceeds 3 minutes, the run is terminated.
- **Trophy Criterion**: The fastest valid **Run 2** time wins the tournament.

---

## 🏎️ 4. Competition Winning Secrets

1. **Clean Silicone Tires**:
   - Dust on competition arenas drastically reduces friction. Clean your wheels with **isopropyl alcohol** immediately before placing the robot in the start box.
2. **Center of Gravity (CoG)**:
   - Place the heavy LiPo battery directly above or slightly forward of the motor axle. A rear-heavy robot wheelies and loses sensor contact during Run 2 acceleration.
3. **Ambient Light Shielding**:
   - Bright spotlights and cameras can saturate IR photodiodes. Add a 3D-printed black hood/shroud around your sensor bar to block ambient overhead light.
