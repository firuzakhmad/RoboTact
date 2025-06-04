# RoboTact [![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

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
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
```

#### macOS
```bash
git clone https://github.com/firuzakhmad/RoboTact.git
cd RoboTact
mkdir build && cd build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

#### Linux
```bash
git clone https://github.com/firuzakhmad/RoboTact.git
cd RoboTact
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Running

After building, launch the application from:

    Windows: build/bin/Release/RoboTact.exe

    macOS: build/bin/Release/RoboTact.app

    Linux: build/bin/RoboTact

Contributing

We welcome contributions! Please see our Contribution Guidelines.
Support

For help, please open an issue.
License

RoboTact is released under the MIT License - see [LICENSE.txt](LICENSE) for details.
