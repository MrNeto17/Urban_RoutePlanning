
# 🚗 Urban Route Planning Tool (FEUP DA Project)
*A C++ tool for optimal driving/walking routes using Dijkstra’s algorithm and graph theory.*

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)]()

## 🔍 Overview
This project implements a **route planning system** for urban mobility, supporting:
- **Driving-only routes** (fastest + independent alternative).
- **Restricted routes** (avoid nodes/segments, include mandatory stops).
- **Hybrid driving-walking routes** with parking nodes and max walking time constraints.

Developed for **FEUP's *Design of Algorithms (DA)* course** (Spring 2025), it leverages **greedy algorithms** (Dijkstra) and graph traversal to optimize sustainable mobility.

## ✨ Features

| **Feature**              | **Description**                                                                 |
|--------------------------|----------------------------------------------------------------------------------|
| **Driving Routes**       | Fastest path + alternative route with no shared nodes/segments.                |
| **Restricted Routes**    | Avoid specific nodes/segments or include mandatory stops.                      |
| **Hybrid Routes**        | Combines driving (to parking) + walking (to destination) with user constraints.|
| **Batch Mode**           | Processes `input.txt` and generates `output.txt` for automated testing.        |
| **Error Handling**       | Handles impossible routes with clear output messages.                          |

## 🗂 Project Structure

```
DA_RoutePlanner-CPP/
├── data/           # Sample datasets (Locations.csv, Distances.csv)
├── docs/           # Doxygen documentation
├── src/            # Source code
│   ├── Graph/         # Graph data structure (vertices, edges)
│   ├── RouteFinder/   # Dijkstra + hybrid route logic
│   ├── FileParser/    # CSV parsing utilities
│   └── main.cpp       # CLI menu entry point
├── CMakeLists.txt  # Build configuration
└── README.md       # You are here!
```

## 🛠️ How to Run

1. **Compile**:

```bash
mkdir build && cd build
cmake .. && make
```

2. Execute:

- **Interactive Mode**:

```bash
./RoutePlanner
```

- **Batch Mode**:  
Edit `input.txt` and run:

```bash
./RoutePlanner batch
```

## 📊 Example Output

**Input (`input.txt`)**
```plaintext
Mode: driving-walking
Source: 1
Destination: 10
MaxWalkTime: 15
AvoidNodes: 3,7
AvoidSegments: (2,5)
```

**Output (`output.txt`)**
```plaintext
Source: 1
Destination: 10
DrivingRoute: 1,4,6(20)
ParkingNode: 6
WalkingRoute: 6,8,10(12)
TotalTime: 32
```

## 📄 License

MIT License. See [LICENSE](LICENSE).

## ❓ Why This Project?

- **Algorithms**: Implements Dijkstra's shortest-path with custom constraints.
- **Sustainability**: Promotes eco-friendly routes (max walking segments).
- **Real-world Relevance**: Mimics GPS systems like Google Maps.

*Developed as part of FEUP's DA course (Spring 2025).*

---

### 🛠 How to Use This File:
1. **Download**: Copy the content above into a new file named `README.md` in your project root.
2. **Customize**: Replace placeholder values (e.g., example routes) with your actual project data.
3. **Add Extras**:
   - For **visuals**, add a `screenshots/` folder with demo images.
   - For **badges**, update the shields.io links if needed.

Let me know if you’d like a ZIP with the file or further customizations! 🎓
