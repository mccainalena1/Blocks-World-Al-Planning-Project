# Blocks World AI Planning (C++)

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

C++ implementation of the classic **Blocks World** AI planning problem. This repo contains **only the C/C++ source and headers**, organized under `src/`. The original report and slides are kept under `results/` for reference.

---

## ⚡ Quickstart

1) **Clone the repo**
```bash
git clone https://github.com/mccainalena1/Blocks-World-Al-Planning-Project.git
cd Blocks-World-Al-Planning-Project
```

2) **Build (example with g++)**
```bash
# adjust file list or use a CMakeLists if you add one later
g++ -std=c++17 -O2 -I./src $(find src -name "*.cpp") -o blocks_world
```

3) **Run**
```bash
./blocks_world
```

> Tip: If your project has a specific entry point (e.g., `main.cpp`), compile that and link supporting sources as needed.

---

## 📂 Repository Structure
```
.
├── src/            # C/C++ sources and headers only
├── results/        # (reference) report/slides exported from coursework
├── .gitignore
├── LICENSE
└── README.md
```

---

## 📝 Notes
- Visual Studio/IDE files and other assets have been removed to keep this repository **C++-only**.
- If you want a portable build, consider adding a simple **CMakeLists.txt** later.

