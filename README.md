# 📦 Blocks World AI Planning  

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)  
[![C++](https://img.shields.io/badge/C++-17-blue)]()  
[![CMake](https://img.shields.io/badge/Build-CMake-green)]()  

## 📖 Project Overview  
C++ implementation of the classic Blocks World AI planning problem. Includes source code, Visual Studio solution files, and deliverables (report, slides). Demonstrates planning strategies and outcomes using both direct compilation and CMake builds.  

## 📂 Repository Structure  
```
├── src/           # C++ source code and solution files  
├── results/       # Report, presentation  
├── figures/       # Images/plots (if any)  
├── CMakeLists.txt # CMake build configuration  
├── LICENSE  
└── README.md  
```  

## ⚡ Quickstart  

### 🔧 Build with g++  
```bash
g++ -std=c++17 src/*.cpp -o blocks_world
./blocks_world
```

### 🛠 Build with CMake (alternative)  
```bash
mkdir build && cd build
cmake ..
cmake --build .
./blocks_world
```

## 📊 Data  
Not applicable — this project is focused on AI planning simulation rather than dataset input.  

## 🧠 Modeling Approach  
Implements Blocks World planning with C++ classes for blocks, arm actions, and environment state transitions.  

## 🎯 Results & Artifacts  
- `results/Blocks World.docx` (report)  
- `results/Final Project.pptx` (slides)  

## 🔁 Reproducibility  
- Code builds with g++ 7+ or any modern C++17 compiler.  
- CMakeLists provided for cross-platform reproducibility.  

## 🚀 Next Steps  
- Add automated test cases under `/tests`  
- Extend planner with heuristic-based search  

## 📜 License  
This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.  

## 👤 Contact  
**Alena McCain** — [LinkedIn](https://www.linkedin.com/in/alena-mccain-815a09136)  
