
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

## Algorithms & Complexity Analysis Resume

1. **Dijkstra's Algorithm (Modified):**

  - Used for finding shortest paths in driving mode

  - Time Complexity: O(|E| + |V|log|V|) with priority queue

  - Space Complexity: O(|V|)

2. **Hybrid Route Finder:**

  - Combines Dijkstra variants for driving/walking segments

  - 1st Phase: Dijkstra from source to all parking nodes

  - 2nd Phase: Dijkstra (walking) from parking nodes to destination

  - Time Complexity: O(k(|E| + |V|log|V|)), where k = parking nodes

3. **Alternative Route Finder:**

  - Runs Dijkstra, removes primary route edges/nodes, then re-runs

  - Time Complexity: 2 × Dijkstra = O(2(|E| + |V|log|V|))

### Optimizations
  - Priority Queue: Min-heap implementation for efficient extraction

  - Graph Pruning: Temporary removal of avoided nodes/segments

  - Memoization: Caching parking node distances for hybrid routes

## 📊 Example Output

### Case 1: Hybrid Route

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

### Case 2: Restricted Route

**Input (`input.txt`)**
```plaintext
Mode: driving
Source: 1
Destination: 10
AvoidNodes: 3,7
AvoidSegments: (5,8)
IncludeNode: 4
```

**Output (`output.txt`)**
```plaintext
Source: 1
Destination: 10
RestrictedDrivingRoute: 1,4,6,9,10(32)
```

### Case 1: Driving Non-Restricted Route

**Input (`input.txt`)**
```plaintext
Mode: driving
Source: 1
Destination: 10
```

**Output (`output.txt`)**
```plaintext
Source: 1
Destination: 10
BestDrivingRoute: 1,3,7,10(25)
AlternativeDrivingRoute: 1,2,5,8,10(28)
```


## Screenshots

### Interactive Interface Example
<img src="https://github.com/MrNeto17/Urban_RoutePlanning/blob/main/photos/test_example.png" width="700" alt="Route visualization">

### CSV Locations Format

<img src="https://github.com/MrNeto17/Urban_RoutePlanning/blob/main/photos/locations.png" width="250" alt="Route visualization">

### CSV Distances Format

<img src="https://github.com/MrNeto17/Urban_RoutePlanning/blob/main/photos/distance.png" width="250" alt="Route visualization">

## 📄 License

MIT License. See [LICENSE](LICENSE).

## ❓ Why This Project?

- **Algorithms**: Implements Dijkstra's shortest-path with custom constraints.
- **Sustainability**: Promotes eco-friendly routes (max walking segments).
- **Real-world Relevance**: Mimics GPS systems like Google Maps.

*Developed as part of FEUP's DA course (Spring 2025).*

