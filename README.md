# RoboTact - Open-Source Robotics Learning Platform

![RoboTact Logo](https://via.placeholder.com/150) *(Consider adding a logo here)*

RoboTact is an open-source GUI application designed to make robotics education accessible to everyone. It provides interactive projects with step-by-step guidance to help users learn robotics concepts hands-on.

## Features
- Interactive robotics tutorials
- Visual programming environment
- Simulation capabilities
- Project-based learning
- Cross-platform support

## Getting Started

### Prerequisites
- **CMake** 3.16+
- **C++17** compatible compiler
- **OpenGL** 3.3+ support
- **Git**

### Installation

#### Windows
```bash
git clone https://github.com/firuzakhmad/RoboTact.git
cd RoboTact
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
