# 🏁 Line Maze Layouts, Rules & Test Tracks

Design guidelines, standard competition rules, and test track configurations for practicing line maze navigation.

---

## 📏 Standard Maze Specifications

- **Line Width**: 19 mm (~0.75 inches) black electrical tape on white background (or white on black).
- **Grid Spacing**: Minimum 15 cm to 25 cm between parallel lines and adjacent junctions.
- **Turn Radius**: Sharp 90° right-angle intersections and T-junctions.
- **Start Mark**: Marked line with crossbar or designated start box.
- **Finish Mark**: A solid black square or T-junction marked with a continuous line bar.

---

## 🗺️ Test Maze Topologies

### Maze 1: Simple Fork & Dead End (Sanity Check)
```
      [Start]
         |
         +-------(Dead End)
         |
      [Finish]
```
- Exploration path: `L` (dead end) $\to$ `B` $\to$ `S`
- Simplified path: `R`

### Maze 2: Double Dead End Tree
```
              [Finish]
                 |
        +--------+
        |        |
    (Dead End)   +-------(Dead End)
                 |
              [Start]
```
- Explores all branches using Left-Hand-Rule.
- Verifies real-time simplification: reduces dead branches to direct trajectory to Goal.

