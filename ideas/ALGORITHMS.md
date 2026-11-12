# 🧠 Line Maze Solving Algorithms & Mathematics

This document details the algorithmic logic powering the **Line Maze Solver**, from basic line following to full maze mapping and shortest-path optimization.

---

## 1. Line Tracking with PID Control

Closed-loop PID keeps the robot centered on the line between maze junctions.

### Formula
$$\text{Error} = \text{Target Position} - \text{Current Position}$$
$$P = K_p \times \text{Error}$$
$$I = K_i \times \sum (\text{Error} \times \Delta t)$$
$$D = K_d \times \frac{\text{Error} - \text{Previous Error}}{\Delta t}$$
$$\text{Correction} = P + I + D$$

### Motor Command Allocation
- $\text{Speed}_{\text{Left}} = \text{BaseSpeed} + \text{Correction}$
- $\text{Speed}_{\text{Right}} = \text{BaseSpeed} - \text{Correction}$

---

## 2. LSRB (Left-Straight-Right-Back) Algorithm

### Decision Hierarchy (Left-Hand Rule)
At every intersection or dead-end, the robot makes a decision based on strict priority:
1. **Turn Left (`L`)**: Always take a left branch if one exists.
2. **Go Straight (`S`)**: If no left branch is present, continue straight.
3. **Turn Right (`R`)**: If left and straight are unavailable, turn right.
4. **Turn Back (`B`)**: If no path is available (Dead End), rotate 180°.

---

## 3. Mathematical Proof: The Angle Sum Theorem for Path Reduction

Why does path reduction work consistently? Every turn can be modeled as a vector rotation in the 2D plane:

- **Straight (`S`)**: $0^\circ$ rotation
- **Right Turn (`R`)**: $+90^\circ$ rotation
- **Turn Back / U-Turn (`B`)**: $+180^\circ$ rotation
- **Left Turn (`L`)**: $+270^\circ$ (or $-90^\circ$) rotation

Whenever a dead end occurs, the sequence consists of three moves: $[M_1] \to [B] \to [M_2]$.
The net resultant angle is:
$$\theta_{\text{net}} = (\theta_{M1} + 180^\circ + \theta_{M2}) \pmod{360^\circ}$$

### Reduction Matrix Verification

| Triplet ($M_1 - B - M_2$) | Angle Calculation | $\theta_{\text{net}}$ | Simplified Move | Physical Interpretation |
|---|---|---|---|---|
| `L - B - L` | $270^\circ + 180^\circ + 270^\circ = 720^\circ$ | $0^\circ$ | **`S`** | Turning left into a dead end and turning left out equals going straight |
| `L - B - S` | $270^\circ + 180^\circ + 0^\circ = 450^\circ$ | $90^\circ$ | **`R`** | Turning left into a dead end and continuing straight equals right turn |
| `R - B - L` | $90^\circ + 180^\circ + 270^\circ = 540^\circ$ | $180^\circ$ | **`B`** | Right into dead end and left equals another dead end |
| `S - B - L` | $0^\circ + 180^\circ + 270^\circ = 450^\circ$ | $90^\circ$ | **`R`** | Straight into dead end and left equals right turn |
| `S - B - S` | $0^\circ + 180^\circ + 0^\circ = 180^\circ$ | $180^\circ$ | **`B`** | Straight into dead end and straight equals dead end |
| `L - B - R` | $270^\circ + 180^\circ + 90^\circ = 540^\circ$ | $180^\circ$ | **`B`** | Left into dead end and right equals U-turn |

---

## 4. Junction Detection & "Inching" Mechanics

The sensor array sits in front of the wheel axis. When outer sensors detect a branch:
1. **Trigger**: Left or right sensors read black (`1`).
2. **Inch Forward**: The robot drives straight for $\sim 100-150\text{ ms}$ (the distance between sensor bar and wheel pivot axis).
3. **Re-sample**:
   - If line is still present in the center $\implies$ A straight line continues forward.
   - If all sensors read black $\implies$ Goal / End of Maze marker.
   - If center is empty $\implies$ Only the turn was available (90° corner or T-junction).
4. **Execute**: Turn the robot until the center sensor locks back onto the line.

---

## 5. Advanced Alternative: Trémaux's Algorithm (For Mazes With Loops)

*LSRB guarantees solution on tree-structured mazes (no closed loops). For mazes with closed loops or islands:*
- Use wheel encoders + IMU (MPU6050) to generate junction $(X, Y)$ coordinate nodes.
- Mark edges with visit count:
  - 0 visits: unvisited passage (highest priority).
  - 1 visit: passage traversed once (turn back if a second visit happens).
  - 2 visits: passage exhausted.
