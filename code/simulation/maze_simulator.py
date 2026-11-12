"""
Interactive Python Line Maze Solver & LSRB Simulator
Features:
- Complete LSRB Reduction Engine (Angle Arithmetic & Substitution Matrix)
- Comprehensive Test Suite for all 6 reduction rules
- Dual-Phase Simulation (Run 1: Left-Hand Exploration, Run 2: High-Speed Shortest Path)
"""

import sys

# Reduction matrix mapping any triplet containing a dead-end 'B' to its optimal equivalent
LSRB_REDUCTION_MAP = {
    ("L", "B", "L"): "S",
    ("L", "B", "S"): "R",
    ("R", "B", "L"): "B",
    ("S", "B", "L"): "R",
    ("S", "B", "S"): "B",
    ("L", "B", "R"): "B",
}

# Equivalent angle mapping (Degrees)
ANGLE_MAP = {"S": 0, "R": 90, "B": 180, "L": 270}
REVERSE_ANGLE_MAP = {0: "S", 90: "R", 180: "B", 270: "L"}


def simplify_using_angles(move1: str, move2: str, move3: str) -> str:
    """
    Computes path reduction using modular angle arithmetic:
    Total Angle = (A1 + A2 + A3) mod 360
    """
    total_angle = (ANGLE_MAP[move1] + ANGLE_MAP[move2] + ANGLE_MAP[move3]) % 360
    return REVERSE_ANGLE_MAP[total_angle]


def simplify_path(path_list: list) -> list:
    """
    Iteratively reduces all dead-end 'B' occurrences in the path until optimal.
    """
    path = list(path_list)
    changed = True
    while changed:
        changed = False
        if len(path) < 3:
            break
        for i in range(len(path) - 2):
            triplet = (path[i], path[i + 1], path[i + 2])
            if triplet[1] == "B" and triplet in LSRB_REDUCTION_MAP:
                replacement = LSRB_REDUCTION_MAP[triplet]
                print(f"  ⚡ [Reducer] Cancelled Dead-End: {'-'.join(triplet)} => {replacement}")
                path = path[:i] + [replacement] + path[i + 3:]
                changed = True
                break
    return path


def run_unit_tests():
    print("=" * 65)
    print("🧪 RUNNING LSRB ALGORITHM UNIT TESTS (ALL 6 REDUCTIONS)")
    print("=" * 65)
    all_passed = True
    for (m1, b, m3), expected in LSRB_REDUCTION_MAP.items():
        calculated_by_table = LSRB_REDUCTION_MAP[(m1, b, m3)]
        calculated_by_angle = simplify_using_angles(m1, b, m3)

        assert calculated_by_table == expected
        assert calculated_by_angle == expected

        print(f"  [PASS] {m1} + {b} + {m3} -> {expected}  (Angle Sum: {simplify_using_angles(m1, b, m3)})")

    print("\n✅ All 6 LSRB reduction rules verified successfully with angle proof!\n")


def run_dual_phase_maze_simulation():
    print("=" * 65)
    print("🤖 DUAL-PHASE LINE MAZE SOLVER SIMULATION")
    print("=" * 65)

    # Simulated exploration sequence through a multi-branch maze with 2 dead ends
    raw_exploration_trace = [
        ("Junction 1", "L"),
        ("Dead End A", "B"),
        ("Junction 1", "S"),  # L-B-S => R
        ("Junction 2", "L"),
        ("Dead End B", "B"),
        ("Junction 2", "L"),  # L-B-L => S
        ("Junction 3", "S"),  # Goal
    ]

    print("\n📍 PHASE 1: EXPLORATION (LSRB Left-Hand Rule)")
    print("The robot encounters unknown junctions, logs choices, and cancels dead ends in real-time.\n")

    current_path = []
    for loc, move in raw_exploration_trace:
        current_path.append(move)
        print(f"  At {loc:<12} -> Action: [{move}] | Path: {' -> '.join(current_path)}")
        if len(current_path) >= 3 and current_path[-2] == "B":
            current_path = simplify_path(current_path)
            print(f"  ==> Real-time simplified path: {' -> '.join(current_path)}\n")

    raw_moves = [m for _, m in raw_exploration_trace]
    print("=" * 65)
    print("🏁 EXPLORATION COMPLETED (FINISH LINE DETECTED)")
    print(f"  Raw Moves Taken:          {' -> '.join(raw_moves)}  ({len(raw_moves)} turns)")
    print(f"  Optimized Shortest Path:  {' -> '.join(current_path)}  ({len(current_path)} turns)")
    print("=" * 65)

    print("\n🚀 PHASE 2: HIGH-SPEED RUN (RUN 2)")
    print("Robot resets to start. Replays the pre-computed optimal path at maximum acceleration:\n")

    action_names = {"L": "Turn LEFT ⬅️", "R": "Turn RIGHT ➡️", "S": "Shoot STRAIGHT ⬆️", "B": "Turn BACK 🔄"}
    for idx, action in enumerate(current_path, 1):
        print(f"  Speed Run Node #{idx}: Executing [{action}] ({action_names.get(action)}) at 100% Throttle ⚡")

    print("\n🏆 FINISH LINE REACHED IN MINIMUM TIME!")


if __name__ == "__main__":
    run_unit_tests()
    run_dual_phase_maze_simulation()
