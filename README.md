# 🤖 Line Maze Solver Robot

An autonomous Line Maze Solving Robot project designed to navigate unknown line mazes, map junctions, simplify paths using the **LSRB (Left-Straight-Right-Back)** algorithm, and execute an optimized high-speed run to the finish line.

---

## 📂 Project Organization

This repository is structured into clear, dedicated folders:

```
line-maze-solver/
├── README.md                      # Complete project documentation and overview
├── .gitignore                     # Git ignore rules for firmware, Python, and IDE files
├── sync_github.py                 # Automated GitHub sync and repository updater
├── code/                          # All executable code and software
│   ├── firmware/
│   │   └── line_maze_solver.ino   # Arduino / ESP32 C++ firmware with PID & LSRB
│   └── simulation/
│       └── maze_simulator.py      # Python interactive visual maze simulator
├── ideas/                         # Design documents, algorithms, and roadmaps
│   ├── PROJECT_IDEAS.md           # Master ideas log, dual-run strategy, and future goals
│   ├── ALGORITHMS.md              # In-depth math for PID, LSRB reduction table, and Trémaux's
│   └── HARDWARE_CIRCUIT.md        # Pinout tables, motor driver wiring, and power architecture
└── others/                        # Reference materials, test tracks, and calibration
    ├── MAZE_DESIGNS.md            # Standard competition maze specs and sample topologies
    └── CALIBRATION_GUIDE.md       # Step-by-step IR sensor threshold calibration
```

---

## 🌟 Key Features

- **Autonomous Maze Exploration (Run 1)**: Traverses unknown grid/line mazes using the Left-Hand on Wall rule.
- **Junction Detection**: Identifies intersections:
  - Left turns (`L`)
  - Right turns (`R`)
  - T-junctions (`T`)
  - Cross intersections (`+`)
  - Dead ends (`B` / Turn Back)
  - End of maze / Goal (`E`)
- **Path Simplification Engine**: Real-time path optimization that cancels out dead ends (e.g., `L-B-L` $\to$ `S`, `L-B-S` $\to$ `R`, `R-B-L` $\to$ `B`).
- **Shortest Path Speed Run (Run 2)**: Re-runs the maze at high speed by executing only the simplified optimal path with zero dead ends.
- **PID Line Following**: Smooth proportional-integral-derivative control for high-speed line tracking between junctions.
- **Python Simulator**: Interactive desktop simulation to test algorithms before deploying to physical hardware.

---

## 📐 Maze Solving Algorithm (LSRB)

### 1. Left Hand Rule Priority
At any junction or decision point, the robot prioritizes moves in the following order:
1. **Left** (highest priority)
2. **Straight**
3. **Right**
4. **Back** (turn around at dead end)

### 2. Path Reduction Matrix
When the robot encounters a dead end (`B`), it simplifies the path string using the substitution table:

| Unoptimized Path | Simplified Move | Explanation |
|---|---|---|
| `L` + `B` + `L` | `S` (Straight) | Left turn, dead end, left turn equals going straight |
| `L` + `B` + `S` | `R` (Right) | Left turn, dead end, straight equals right turn |
| `R` + `B` + `L` | `B` (Back) | Right turn, dead end, left equals dead end |
| `S` + `B` + `L` | `R` (Right) | Straight, dead end, left equals right turn |
| `S` + `B` + `S` | `B` (Back) | Straight, dead end, straight equals dead end |
| `L` + `B` + `R` | `B` (Back) | Left turn, dead end, right equals U-turn |

---

## 🚀 Getting Started

### 1. Python Maze Simulator
Run the simulator to visualize path solving and reduction:
```bash
python code/simulation/maze_simulator.py
```

### 2. Physical Robot Firmware (Arduino / ESP32)
1. Open [code/firmware/line_maze_solver.ino](file:///d:/line maze solver/code/firmware/line_maze_solver.ino) in Arduino IDE or PlatformIO.
2. Verify pin assignments in [ideas/HARDWARE_CIRCUIT.md](file:///d:/line maze solver/ideas/HARDWARE_CIRCUIT.md).
3. Follow calibration steps in [others/CALIBRATION_GUIDE.md](file:///d:/line maze solver/others/CALIBRATION_GUIDE.md).

### 3. Syncing to GitHub
To push or update the repository on GitHub (`padmanaban1908/line-maze-solver`):
```bash
python sync_github.py <YOUR_GITHUB_TOKEN>
```
*(Direct link to generate token with repo scope: [GitHub Token Generator](https://github.com/settings/tokens/new?scopes=repo&description=line-maze-solver))*
