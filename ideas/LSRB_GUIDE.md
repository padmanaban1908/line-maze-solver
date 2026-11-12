# 📘 Complete Guide to the LSRB Algorithm for Line Maze Solvers

The **LSRB Algorithm** (Left, Straight, Right, Back) is the gold-standard navigation and path-simplification algorithm used by autonomous line-following robots to map unknown line mazes in **Run 1 (Exploration)** and race through the shortest optimal path in **Run 2 (Speed Run)**.

---

## 🧭 1. What Does LSRB Stand For?

| Letter | Action | Priority | When Chosen |
|:---:|:---|:---:|:---|
| **L** | **Turn Left** | **1 (Highest)** | If a left branch exists, **always turn left**. |
| **S** | **Go Straight** | **2** | If you cannot go left, but can continue straight, go straight. |
| **R** | **Turn Right** | **3** | If you cannot go left or straight, turn right. |
| **B** | **Turn Back (180°)** | **4 (Lowest)** | If no path is available (Dead End), turn 180° around. |

> **Analogy:** This is the digital equivalent of placing your left hand on the wall of a maze and walking without ever lifting your hand. In a maze with no closed loops (a tree-graph), the left-hand rule **guarantees** reaching the exit.

---

## ⚙️ 2. The Two-Run Competition Strategy

1. **Run 1 (Exploration)**:
   - The robot knows nothing about the layout.
   - At every intersection, it picks according to **L $\to$ S $\to$ R $\to$ B**.
   - It explores dead ends, records every choice into an array, and cleans up dead ends on the fly.
2. **Run 2 (Speed Run)**:
   - You place the robot back at the start line.
   - The robot ignores the exploration logic and reads only the **pre-simplified shortest path**.
   - It flies through every junction at maximum throttle with **0 dead ends visited**.

---

## 🔍 3. How the Robot Detects Junctions: The "Inching" Technique

Because the IR sensor array is mounted **in front of the wheels**, when a sensor hits a cross-line, the wheels are still 2–4 cm behind the intersection:

### The Inching Sequence:
1. **Trigger**: Outer left or right sensor sees black tape (`1 1 1 0 0` or `0 0 1 1 1`).
2. **Inch Forward**: Robot drives straight for $\sim 100-150\text{ ms}$ until the **wheel axis sits right on the center of the junction**.
3. **Re-sample Line**:
   - If middle sensor still sees black $\implies$ **Straight path is available**.
   - If middle sensor sees white $\implies$ **No straight path** (it was a pure 90° corner or T-junction).
   - If all sensors see black $\implies$ **End of Maze (Finish Box)**.
4. **Execute & Record**: The robot executes the highest-priority available turn according to LSRB, appends the letter to its path array, and calls `simplifyPath()`.

---

## ✂️ 4. Path Simplification: How Dead Ends Are Eliminated

Whenever the robot hits a dead end, it turns around (**`B`**). A dead end always creates a 3-step sequence:  
$$\text{[Turn Into Dead End]} \longrightarrow \mathbf{B} \longrightarrow \text{[Turn Out of Dead End]}$$

These 3 moves can always be compressed into a single optimal move using the **LSRB Reduction Matrix**:

| Triplet Sequence | Compressed Move | Visual Meaning |
|:---:|:---:|:---|
| **`L - B - L`** | $\implies$ **`S`** | Turning left into a dead end and left on the way out equals going straight! |
| **`L - B - S`** | $\implies$ **`R`** | Turning left into a dead end and going straight out equals a right turn! |
| **`R - B - L`** | $\implies$ **`B`** | Right into a dead end, then left out means the whole corridor was a dead end. |
| **`S - B - L`** | $\implies$ **`R`** | Straight into a dead end, then left out equals a right turn. |
| **`S - B - S`** | $\implies$ **`B`** | Straight into a dead end, then straight out means a dead end. |
| **`L - B - R`** | $\implies$ **`B`** | Left into dead end, then right out equals a U-turn (dead end). |

---

## 📐 5. Mathematical Proof: The Angle Sum Theorem

Why does path reduction work every single time? Because turns are geometric rotations in 2D space:

$$\text{Straight (S)} = 0^\circ \qquad \text{Right (R)} = 90^\circ \qquad \text{Back (B)} = 180^\circ \qquad \text{Left (L)} = 270^\circ \ (-90^\circ)$$

When a dead-end sequence $[M_1] \to [B] \to [M_2]$ occurs:
$$\theta_{\text{net}} = (\theta_{M1} + 180^\circ + \theta_{M2}) \pmod{360^\circ}$$

### Applying the Math:
- **`L - B - L`**: $(270^\circ + 180^\circ + 270^\circ) = 720^\circ \equiv \mathbf{0^\circ} \implies \mathbf{S}$
- **`L - B - S`**: $(270^\circ + 180^\circ + 0^\circ) = 450^\circ \equiv \mathbf{90^\circ} \implies \mathbf{R}$
- **`R - B - L`**: $(90^\circ + 180^\circ + 270^\circ) = 540^\circ \equiv \mathbf{180^\circ} \implies \mathbf{B}$
- **`S - B - L`**: $(0^\circ + 180^\circ + 270^\circ) = 450^\circ \equiv \mathbf{90^\circ} \implies \mathbf{R}$

---

## 🏁 6. Step-by-Step Example Walkthrough

```
[Start]
   │
   ├────── (Dead End A)
   │
   ├────── (Dead End B)
   │
[Goal]
```

1. **At Junction 1**: Robot chooses **`L`** (enters Dead End A).
2. **At Dead End A**: No line $\implies$ Turns around (**`B`**).
3. **Returns to Junction 1**: Takes next available move: **`S`**.
   - Path so far: `['L', 'B', 'S']`
   - *Optimization triggers*: `L - B - S` $\implies$ **`R`**.  
   - Current saved path: **`['R']`**
4. **At Junction 2**: Chooses **`L`** (enters Dead End B).
5. **At Dead End B**: Turns around (**`B`**).
6. **Returns to Junction 2**: Takes next move: **`L`**.
   - Path recorded: `['R', 'L', 'B', 'L']`
   - *Optimization triggers*: `L - B - L` $\implies$ **`S`**.  
   - Current saved path: **`['R', 'S']`**
7. **At Junction 3**: Shoots straight to the Goal (**`S`**).
   - Final optimal path: **`['R', 'S', 'S']`**

### Summary:
- **Run 1 (Exploration)**: 7 movements (`L -> B -> S -> L -> B -> L -> S`).
- **Run 2 (Speed Run)**: Exactly 3 movements (`R -> S -> S`). **Zero dead ends visited!**
